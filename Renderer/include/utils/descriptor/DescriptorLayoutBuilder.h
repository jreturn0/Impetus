#pragma once
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"

namespace imp::gfx
{
    // Builder class for creating Vulkan descriptor set layouts
    class DescriptorLayoutBuilder
    {

    public:
        DescriptorLayoutBuilder() = default;
        DescriptorLayoutBuilder(const DescriptorLayoutBuilder&) = delete; 
        DescriptorLayoutBuilder& operator=(const DescriptorLayoutBuilder&) = delete; 
        DescriptorLayoutBuilder(DescriptorLayoutBuilder&&) = delete; 
        DescriptorLayoutBuilder& operator=(DescriptorLayoutBuilder&&) = delete;;

        // Getters

        inline const std::vector<vk::DescriptorSetLayoutBinding>& getBindings() const noexcept { return bindings; };

        // Methods

        void addBinding(uint32_t binding, vk::DescriptorType type);
        vk::raii::DescriptorSetLayout build(const vk::raii::Device& device, vk::ShaderStageFlags stageFlags, void* pNext=nullptr, vk::DescriptorSetLayoutCreateFlags={}) ;
        void clear();
    private:
        std::vector<vk::DescriptorSetLayoutBinding> bindings{};
    };
}
