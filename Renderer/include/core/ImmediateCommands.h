#pragma once
#include <functional>
#include <optional>
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace imp::gfx {



    // Multi-threaded one time/immediate command buffer submission utility
    class ImmediateCommands
    {
    public:
        ImmediateCommands(const vk::raii::Device& device, uint32_t familyIndex);
        ~ImmediateCommands();
        ImmediateCommands(const ImmediateCommands&) = delete;
        ImmediateCommands& operator=(const ImmediateCommands&) = delete;
        ImmediateCommands(ImmediateCommands&&) = delete;
        ImmediateCommands& operator=(ImmediateCommands&&) = delete;


        void submit(vk::Queue queue, std::function<void(vk::CommandBuffer cmd)>&& function) const;

    private:
        const vk::raii::Device* m_device{ nullptr };
        uint32_t m_familyIndex{ 0 };
    };


}
