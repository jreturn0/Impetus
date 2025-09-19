#pragma once
#include <functional>
#include <queue>
#include "Buffer.h"
#include "utils/descriptor/DescriptorAllocatorGrowable.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "utils/Forwards.h"

namespace imp::gfx {
    using FrameDataArray = std::array<std::unique_ptr<class FrameData>, vkutil::MAX_FRAMES_IN_FLIGHT>;
    class FrameData
    {
    public:
        FrameData() = default;
        ~FrameData();

        // Disable copy and move
        FrameData(const FrameData&) = delete;
        FrameData& operator=(const FrameData&) = delete;
        FrameData(FrameData&&) = delete;
        FrameData& operator=(FrameData&&) = delete;


        // Getters and Setters

        const Buffer& getSceneDataBuffer() const { return m_sceneDataBuffer; }
        vk::DescriptorSet getSceneDataDescriptorSet() const { return m_sceneDataDescriptorSet; };
        vk::Semaphore getImageAvailableSemaphore() const { return m_imageAvailable; }
        const vk::Semaphore& getRenderFinishedSemaphore() const { return m_renderFinished; }
        vk::Fence getInFlightFence() const { return m_inFlightFence; }
        vk::CommandBuffer getCommandBuffer() const { return m_commandBuffer; }
        DescriptorAllocatorGrowable& getDescriptorAllocator() { return m_descriptorAllocator; }

        void setSceneDataBuffer(Buffer&& buffer) { m_sceneDataBuffer = std::move(buffer); }


        void createNewSceneDataDescriptorSet(const vk::raii::Device& device, vk::DescriptorSetLayout layout)
        {
            m_sceneDataDescriptorSet = m_descriptorAllocator.allocate(device, layout);
        }

        // Defer deletion of resources until the end of the frame

        void flushDeletions() { while (!deferredDeletions.empty()) { deferredDeletions.front()(); deferredDeletions.pop(); } }
        void deferDelete(std::function<void()>&& func) { deferredDeletions.push(std::move(func)); }
    private:
        vk::raii::Semaphore m_imageAvailable{ nullptr };
        vk::raii::Semaphore m_renderFinished{ nullptr };
        vk::raii::Fence m_inFlightFence{ nullptr };

        vk::raii::CommandBuffer m_commandBuffer{ nullptr };
        Buffer m_sceneDataBuffer{};
        DescriptorAllocatorGrowable m_descriptorAllocator{};
        vk::raii::DescriptorSet m_sceneDataDescriptorSet{ nullptr };


        std::queue<std::function<void()>> deferredDeletions;

        friend 	FrameDataArray CreateFrameDataArray(const vk::raii::Device& device,
            const VmaAllocator allocator,
            const SwapChain& swapChain,
            const vk::raii::CommandPool& graphicsCommandPool);


    };


    FrameDataArray CreateFrameDataArray(const vk::raii::Device& device,
        const VmaAllocator allocator,
        const SwapChain& swapChain,
        const vk::raii::CommandPool& graphicsCommandPool);
}

