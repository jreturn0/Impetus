#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "SwapChainSupportDetails.h"



namespace Imp {
	class Window;
	namespace Render {
		class Device;
		class VKWindow;
		class RenderPass;

		class SwapChain
		{
		private:
			SwapChainSupportDetails details;
			vk::UniqueSwapchainKHR swapChain;
			std::vector<vk::Image> images;
			std::vector<vk::UniqueImageView> imageViews;
			vk::Format format;
			vk::Extent2D extent;
			std::vector<vk::UniqueFramebuffer> framebuffers;
			
		


			void CreateSwapChain(const Device& device, const VKWindow& window );
			void CreateImageViews(const Device& device);
		public:
			explicit SwapChain(const Device& device, const VKWindow& window);
			SwapChain ();
			~SwapChain();
			DISABLE_COPY_AND_MOVE(SwapChain);
			[[nodiscard]] const SwapChainSupportDetails& getDetails() const { return details; }
			[[nodiscard]] const vk::SwapchainKHR& getSwapChain() const { return *swapChain; }
			[[nodiscard]] const std::vector<vk::Image>& getImages() const { return images; }
			[[nodiscard]] const std::vector<vk::UniqueImageView>& getImageViews() const { return imageViews; }
			[[nodiscard]] vk::Format getFormat() const { return format; }
			[[nodiscard]] vk::Extent2D getExtent() const { return extent; }
			[[nodiscard]] const std::vector<vk::UniqueFramebuffer>& getFramebuffers() const { return framebuffers; }
			

			operator const vk::SwapchainKHR() const { return swapChain.get(); }
			operator vk::SwapchainKHR() { return swapChain.get(); }


			void Cleanup();
			void RecreateSwapChain(const Device& device, const VKWindow& window);

		};
		using UniqueSwapChain = std::unique_ptr<class SwapChain>;
		 UniqueSwapChain CreateUniqueSwapChain(const class Device& device,const VKWindow& window);
	}
}