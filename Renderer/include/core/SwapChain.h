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
		using UniqueSwapChain = std::unique_ptr<class SwapChain>;
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
			
			static  vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
			static  vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
			static  vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const VKWindow& window);

			friend UniqueSwapChain CreateUniqueSwapChain(const  Device& device, const VKWindow& window);
			SwapChain(const Device& device, const VKWindow& window);
			void CreateSwapChain(const Device& device, const VKWindow& window );
			void CreateImageViews(const Device& device);
		public:
			DISABLE_COPY_AND_MOVE(SwapChain);
			const SwapChainSupportDetails& getDetails() const { return details; }
			const vk::SwapchainKHR& getSwapChain() const { return *swapChain; }
			const std::vector<vk::Image>& getImages() const { return images; }
			const std::vector<vk::UniqueImageView>& getImageViews() const { return imageViews; }
			const std::vector<vk::UniqueFramebuffer>& getFramebuffers() const { return framebuffers; }
			const vk::Format getFormat() const { return format; }
			const vk::Extent2D& getExtent() const { return extent; }
			

			inline operator const vk::SwapchainKHR() const { return swapChain.get(); }
			operator vk::SwapchainKHR() { return swapChain.get(); }


			void Cleanup();
			void RecreateSwapChain(const Device& device, const VKWindow& window);

		};
		 UniqueSwapChain CreateUniqueSwapChain(const class Device& device,const VKWindow& window);
	}
}