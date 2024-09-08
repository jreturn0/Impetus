#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "core/Vma.h"


#include "utils/Forwards.h"


namespace Imp::Render {

	class Image
	{
	private:
		Vma* allocator;
		vk::UniqueImage image;
		vk::UniqueImageView view;
		VmaAllocation allocation;
		vk::Extent3D extent;
		vk::Format format;


	public:
		// Create an image without data
		Image(const Device& device, Vma& allocator, vk::Extent3D extent, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap = false);
		// Create an image with data
		Image(const Device& device, Vma& allocator, const Queue& transferQueue, const Queue& graphicsQueue, const CommandPool& transferPool, const CommandPool& graphicsPool,
			  void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap = false);
		Image();
		~Image();
		DISABLE_COPY_AND_MOVE(Image);
		// Getters
		const vk::Image& getImage() const;
		const vk::Image* getpImage() const;
		const vk::ImageView& getView() const;
		const vk::Extent3D& getExtent() const;
		const vk::Extent2D& getExtent2D() const;
		const vk::Format& getFormat() const;

		// Transition image layout
		void transitionImageLayout(const vk::CommandBuffer& commandBuffer, const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout);
		void copyTo(const CommandBuffer& commandBuffer, const Image& dst) const;


	};
	using UniqueImage = std::unique_ptr<Image>;




	using SharedImage = std::shared_ptr<Image>;
	UniqueImage CreateDrawImage(const Device& device, Vma& allocator, vk::Extent2D extent, vk::Format format);
	UniqueImage CreateDepthImage(const Device& device, Vma& allocator, vk::Extent2D extent);

	// Create UniqueImage with data
	UniqueImage CreateImage(const Device& device, Vma& allocator, const Queue& transferQueue, const Queue& graphicsQueue, const CommandPool& transferPool, const CommandPool& graphicsPool,
							void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap = false);
	// Create UniqueImage without data
	UniqueImage CreateImage(const Device& device, Vma& allocator, vk::Extent3D extent, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap = false);
	// Create SharedImage with data
	SharedImage CreateSharedImage(const Device& device, Vma& allocator, const Queue& transferQueue, const Queue& graphicsQueue, const CommandPool& transferPool, const CommandPool& graphicsPool,
								  void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap = false);
	// Create SharedImage without data
	SharedImage CreateSharedImage(const Device& device, Vma& allocator, vk::Extent3D extent, vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectFlags, bool mipmap = false);

	void CopyImageToImage(const CommandBuffer& cmd, Image& src, Image& dst);
	void CopyImageToSwapChain(const vk::CommandBuffer& cmd, Image& src, const SwapChain& swapChain, uint32_t imageIndex);
	void TransitionImageLayout(const vk::Image& image, const vk::CommandBuffer& commandBuffer, const vk::ImageLayout& oldLayout, const vk::ImageLayout
							   & newLayout);
}
