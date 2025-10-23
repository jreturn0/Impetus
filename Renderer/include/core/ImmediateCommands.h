#pragma once
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

namespace imp::gfx {



    // Single submit one time/immediate command buffer submission utility (Multi-threaded friendly)
    class ImmediateCommands
    {
        struct Submission {
            std::function<void(vk::CommandBuffer cmd)> func;
            std::promise<void> promise;
        };
    public:
        ImmediateCommands() = default;
        ImmediateCommands(const vk::raii::Device& device, vk::Queue queue, uint32_t familyIndex);
        ~ImmediateCommands();
        ImmediateCommands(const ImmediateCommands&) = delete;
        ImmediateCommands& operator=(const ImmediateCommands&) = delete;
        ImmediateCommands(ImmediateCommands&&) = delete;
        ImmediateCommands& operator=(ImmediateCommands&&) = delete;

        // Methods

        //void submit(std::function<void(vk::CommandBuffer cmd)>  function) const;
        void submit(std::function<void(vk::CommandBuffer cmd)>  function) const ;
        std::future<void> submitAsync(std::function<void(vk::CommandBuffer cmd)>  function) const ;

    private:
        const vk::raii::Device* m_device{ nullptr };
        vk::Queue m_queue{ nullptr };
        uint32_t m_familyIndex{ 0 };
        std::vector<std::shared_ptr<std::thread>> m_threads{};
        mutable std::queue<Submission> m_submissionQueue{};
        mutable std::condition_variable m_condition{};
        mutable std::mutex m_mutex{};
        mutable std::atomic_bool m_shutdown{ false };
        std::mutex m_queueMutex{};
        std::atomic_size_t m_activeThreads{ 0 };
    };


}
