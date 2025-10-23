#include "ConfigSystem.h"
#include "core/ImmediateCommands.h"
#include "Debug.h"

namespace {
    auto cfg_maxImmediateThreads = utl::ConfigValueRef("renderer.max_immediate_threads", 4);
} // namespace

// ------------------------------------------------------------------------

imp::gfx::ImmediateCommands::ImmediateCommands(const vk::raii::Device& device, vk::Queue queue, uint32_t familyIndex)
    : m_device(&device), m_queue(queue), m_familyIndex(familyIndex)
{
    m_threads.reserve(cfg_maxImmediateThreads.getCopy());
    for (size_t i = 0; i < cfg_maxImmediateThreads.getCopy(); i++) {
        m_threads.emplace_back(std::make_shared<std::thread>([this]() {
            vk::raii::CommandPool   pool{ nullptr };
            vk::raii::CommandBuffer cmd{ nullptr };
            vk::raii::Fence         fence{ nullptr };
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                pool = vk::raii::CommandPool{ m_device->createCommandPool({ vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_familyIndex }) };
                cmd = vk::raii::CommandBuffer{ std::move(m_device->allocateCommandBuffers({ pool, vk::CommandBufferLevel::ePrimary, 1 })[0]) };
                fence = vk::raii::Fence{ m_device->createFence({ vk::FenceCreateFlagBits::eSignaled }) };
                m_activeThreads.fetch_add(1);
            }
            for (;;) {
                Submission submission;
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_condition.wait(lock, [this]() { return m_shutdown.load(std::memory_order_acquire) || !m_submissionQueue.empty(); });
                    if (m_shutdown && m_submissionQueue.empty())
                        break;
                    submission = std::move(m_submissionQueue.front());
                    m_submissionQueue.pop();
                }

                try {
                    m_device->resetFences(*fence);
                    cmd.reset();
                    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
                    cmd.begin(beginInfo);

                    // Record user work
                    submission.func(cmd);

                    cmd.end();
                    vk::CommandBufferSubmitInfo submitInfo(cmd);
                    vk::SubmitInfo2 submitInfo2({}, {}, submitInfo);

                    {
                        std::lock_guard<std::mutex> ql(m_queueMutex);
                        m_queue.submit2(submitInfo2, *fence);
                    }

                    // Signal completion only after GPU finishes
                    vkutil::CheckResult(m_device->waitForFences(*fence, VK_TRUE, UINT64_MAX));
                    submission.promise.set_value();
                }
                catch (const vk::SystemError& e) {
                    Debug::Exception("Vulkan system error in ImmediateCommands: {}", e.what());
                    // Surface errors to the caller instead of hanging
                    try { submission.promise.set_exception(std::make_exception_ptr(e)); }
                    catch (...) {}
                    // On hard failures (e.g., VK_ERROR_DEVICE_LOST), bail out so we stop submitting.
                    // The exception will be visible to the caller via submit()/future.
                    break;
                }
                
                catch (...) {
                    
                    // Surface errors to the caller instead of hanging
                    try { submission.promise.set_exception(std::current_exception()); }
                    catch (...) {}
                    // On hard failures (e.g., VK_ERROR_DEVICE_LOST), bail out so we stop submitting.
                    // The exception will be visible to the caller via submit()/future.
                    break;
                }
            }
            }));
    }
}

imp::gfx::ImmediateCommands::~ImmediateCommands()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_shutdown = true;
    }
    m_condition.notify_all();

    for (auto& t : m_threads) {
        if (t && t->joinable()) t->join();
    }

}

void imp::gfx::ImmediateCommands::submit(std::function<void(vk::CommandBuffer cmd)> function) const
{
    Submission submission;
    submission.func = std::move(function);
    auto future = submission.promise.get_future();

    {
        std::lock_guard lock(m_mutex);
        m_submissionQueue.push(std::move(submission));
    }
    m_condition.notify_one();
    future.wait();
}

std::future<void> imp::gfx::ImmediateCommands::submitAsync(std::function<void(vk::CommandBuffer cmd)> function) const
{
    Submission submission;
    submission.func = std::move(function);
    auto future = submission.promise.get_future();

    {
        std::lock_guard lock(m_mutex);
        m_submissionQueue.push(std::move(submission));
    }
    m_condition.notify_one();

    return future;
}