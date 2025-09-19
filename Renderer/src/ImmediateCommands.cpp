#include "core/ImmediateCommands.h"
#include "Debug.h"

namespace {

    struct ThreadContext {
        vk::raii::CommandPool   pool{ nullptr };
        vk::raii::CommandBuffer cmd{ nullptr };
        vk::raii::Fence         fence{ nullptr };
        inline static size_t liveContexts = 0;


        ThreadContext(const vk::raii::Device& dev, uint32_t familyIndex)
            : pool(dev.createCommandPool({ vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer, familyIndex })),
            cmd(std::move(dev.allocateCommandBuffers({ pool, vk::CommandBufferLevel::ePrimary, 1 })[0])),
            fence(dev.createFence({ vk::FenceCreateFlagBits::eSignaled }))
        {
            ++liveContexts;
        }

        ~ThreadContext() {
            --liveContexts;
            Debug::Info("ThreadContext destroyed, {} contexts still alive", liveContexts);
        }
    };

    std::shared_ptr<ThreadContext>& GetThreadLocalPtr()
    {
        thread_local std::shared_ptr<ThreadContext> ctx = nullptr;
        return ctx;
    }

    class TLSContextManager {
    public:
        TLSContextManager(const TLSContextManager&) = delete;
        TLSContextManager& operator=(const TLSContextManager&) = delete;
        TLSContextManager(TLSContextManager&&) = delete;
        TLSContextManager& operator=(TLSContextManager&&) = delete;

        static TLSContextManager& getInstance() {
            static TLSContextManager instance;
            return instance;
        }

        void registerThread(const std::shared_ptr<ThreadContext>& context) {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto threadId = std::this_thread::get_id();
            if (m_threadContexts.find(threadId) != m_threadContexts.end()) {
                throw std::runtime_error("Thread context already registered");
            }
            m_threadContexts[threadId] = context;
        }

        void cleanup() {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (auto it = m_threadContexts.begin(); it != m_threadContexts.end(); ) {
                if (auto ptr = it->second.lock()) {
                    // Explicitly release Vulkan RAII handles so device can be destroyed safely
                    ptr->cmd = {nullptr};
                    ptr->pool = { nullptr };
                    ptr->fence = { nullptr };
                    // Drop manager reference
                    ptr.reset();
                    it = m_threadContexts.erase(it);
                }
                else {
                    it = m_threadContexts.erase(it);
                }
            }
            // Also clear current thread's TLS pointer if present
            if (auto& tl = GetThreadLocalPtr(); tl) {
                tl.reset();
            }
        }

    private:
        TLSContextManager() = default;
        ~TLSContextManager() = default;

        mutable std::mutex m_mutex;
        mutable std::unordered_map<std::thread::id, std::weak_ptr<ThreadContext>> m_threadContexts;
    };

    std::mutex& QueueSubmitMutex()
    {
        static std::mutex m;
        return m;
    }

    ThreadContext& GetThreadLocalCommands(const vk::raii::Device& dev, uint32_t familyIndex)
    {
        auto& ctx = GetThreadLocalPtr();
        if (!ctx) {
            ctx = std::make_shared<ThreadContext>(dev, familyIndex);
            TLSContextManager::getInstance().registerThread(ctx);
        }
        return *ctx;
    }

} // namespace

// ------------------------------------------------------------------------

imp::gfx::ImmediateCommands::ImmediateCommands(const vk::raii::Device& device, uint32_t familyIndex)
    : m_device(&device), m_familyIndex(familyIndex)
{
}

imp::gfx::ImmediateCommands::~ImmediateCommands()
{
    TLSContextManager::getInstance().cleanup();
}

void imp::gfx::ImmediateCommands::submit(vk::Queue queue, std::function<void(vk::CommandBuffer cmd)>&& function) const
{
    auto& tlc = GetThreadLocalCommands(*m_device, m_familyIndex);

    m_device->resetFences(*tlc.fence);

    tlc.cmd.reset();
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    tlc.cmd.begin(beginInfo);

    function(tlc.cmd);

    tlc.cmd.end();

    vk::CommandBufferSubmitInfo submitInfo(tlc.cmd);
    vk::SubmitInfo2 submitInfo2({}, {}, submitInfo);

    {
        std::lock_guard<std::mutex> lock(QueueSubmitMutex());
        queue.submit2(submitInfo2, *tlc.fence);
    }

    vkutil::CheckResult(m_device->waitForFences(*tlc.fence, VK_TRUE, UINT64_MAX));
}
