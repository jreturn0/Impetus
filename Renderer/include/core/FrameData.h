#pragma once
#include "Buffer.h"
#include "SwapChain.h"
#include "utils/descriptor/DescriptorAllocatorGrowable.h"
#include "utils/VKCommon.hpp"
#include <array>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace imp::gfx {    
    using FrameDataArray = std::array<std::unique_ptr<class FrameData>, vkutil::MAX_FRAMES_IN_FLIGHT>;

    // Represents and stores per-frame resources
    class FrameData
    {
    public:
        FrameData() = default;
        ~FrameData();
        FrameData(const FrameData&) = delete;
        FrameData& operator=(const FrameData&) = delete;
        FrameData(FrameData&&) = delete;
        FrameData& operator=(FrameData&&) = delete;


        // Getters and Setters

        inline const Buffer& getSceneDataBuffer() const noexcept { return m_sceneDataBuffer; }
        inline const Buffer& getLightDataBuffer() const noexcept { return m_lightDataBuffer; }
        inline vk::DescriptorSet getSceneDataDescriptorSet() const noexcept { return m_sceneDataDescriptorSet; }
        inline vk::Fence getInFlightFence() const noexcept { return m_inFlightFence; }
        inline vk::CommandBuffer getCommandBuffer() const noexcept { return m_commandBuffer; }
        inline DescriptorAllocatorGrowable& getDescriptorAllocator() noexcept { return m_descriptorAllocator; }
        inline void setSceneDataBuffer(Buffer&& buffer) { m_sceneDataBuffer = std::move(buffer); }

        // Methods

        inline void createNewSceneDataDescriptorSet(const vk::raii::Device& device, vk::DescriptorSetLayout layout) { m_sceneDataDescriptorSet = m_descriptorAllocator.allocate(device, layout); }
        void flushDeletions() { while (!deferredDeletions.empty()) { deferredDeletions.front()(); deferredDeletions.pop(); } }
        void deferDelete(std::function<void()>&& func) { deferredDeletions.push(std::move(func)); }
    private:
        vk::raii::Fence m_inFlightFence{ nullptr };
        vk::raii::CommandBuffer m_commandBuffer{ nullptr };
        Buffer m_sceneDataBuffer{};
        Buffer m_lightDataBuffer{};
        DescriptorAllocatorGrowable m_descriptorAllocator{};
        vk::raii::DescriptorSet m_sceneDataDescriptorSet{ nullptr };
        std::queue<std::function<void()>> deferredDeletions{};
        friend 	FrameDataArray CreateFrameDataArray(const vk::raii::Device& device,
            const VmaAllocator allocator,
            const SwapChain& swapChain,
            const vk::raii::CommandPool& graphicsCommandPool,
            const vk::raii::DescriptorSetLayout& layout);
    };

    // Factory function to create an array of FrameData for each frame in flight
    FrameDataArray CreateFrameDataArray(const vk::raii::Device& device,
        const VmaAllocator allocator,
        const SwapChain& swapChain,
        const vk::raii::CommandPool& graphicsCommandPool,
        const vk::raii::DescriptorSetLayout& layout);
}

