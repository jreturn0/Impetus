#pragma once
#include <functional>
#include <queue>

#include "CommandBuffer.h"
#include "Buffer.h"
#include "utils/descriptor/DescriptorAllocatorGrowable.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render {
	using UniqueFrameData = std::unique_ptr<class FrameData>;

	using FrameDataArray = std::array<UniqueFrameData, vkutil::MAX_FRAMES_IN_FLIGHT>;
	class FrameData
	{
	private:
		vk::UniqueSemaphore imageAvailable, renderFinished;
		vk::UniqueFence inFlightFence;

		std::unique_ptr<class CommandBuffer> commandBuffer;

		friend 	FrameDataArray CreateFrameDataArray(const class Device& device,
													const class SwapChain& swapChain,
													const class CommandPool& graphicsCommandPool);
		DescriptorAllocatorGrowable descriptorAllocator;

		std::queue<std::function<void()>> deferredDeletions;

		UniqueBuffer sceneDataBuffer;
		vk::UniqueDescriptorSet sceneDataDescriptorSet;
	public:
		FrameData() = default;
		~FrameData() ;
		DISABLE_COPY_AND_MOVE(FrameData);

		Buffer& getSceneDataBuffer() { return *sceneDataBuffer; };
		auto& getSceneDataBuffer_() { return sceneDataBuffer; };
		vk::DescriptorSet getSceneDataDescriptorSet() { return*sceneDataDescriptorSet; };
		auto& getSceneDataDescriptorSet_() { return sceneDataDescriptorSet; };

		void flushDeletions() { while (!deferredDeletions.empty()) { deferredDeletions.front()(); deferredDeletions.pop(); } }
		void deferDelete(std::function<void()>&& func) { deferredDeletions.push(std::move(func)); }


		const vk::Semaphore& getImageAvailableSemaphore() const { return *imageAvailable; }
		const vk::Semaphore& getRenderFinishedSemaphore() const { return *renderFinished; }
		const vk::Fence& getInFlightFence() const { return *inFlightFence; }
		const vk::CommandBuffer& getCommandBuffer() const { return *commandBuffer; }
		const CommandBuffer& getCommandBuffer_() const { return *commandBuffer; }
		DescriptorAllocatorGrowable& getDescriptorAllocator()  { return descriptorAllocator; }

	};


	FrameDataArray CreateFrameDataArray(const class Device& device,
										const class SwapChain& swapChain,
										const class CommandPool& graphicsCommandPool);
}

