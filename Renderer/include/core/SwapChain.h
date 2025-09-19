#pragma once

#include "SwapChainSupportDetails.h"
#include <core/VulkanContext.h>
#include <window/VKWindow.h>

namespace imp::gfx
{

    class SwapChain
    {
    public:
        SwapChain() = default;
        SwapChain(const VulkanContext& context, const Window& window);
        ~SwapChain() = default;
        SwapChain(const SwapChain&) = delete;
        SwapChain& operator=(const SwapChain&) = delete;
        SwapChain(SwapChain&&) = delete;
        SwapChain& operator=(SwapChain&&) = delete;

       /* SwapChain(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const VKWindow& window,
            vk::PresentModeKHR preferredMode = vk::PresentModeKHR::eMailbox);*/


        void recreateSwapChain(const VulkanContext& context, const Window& window);

        // Getters
        const SwapChainSupportDetails& getDetails() const { return m_supportDetails; }
        const vk::SurfaceFormatKHR& getSurfaceFormat() const { return m_surfaceFormat; }
        const vk::PresentModeKHR& getPresentMode() const { return m_presentMode; }
        const vk::Extent2D& getExtent() const { return m_extent; }
        const vk::raii::SwapchainKHR& getSwapChain() const { return m_swapChain; }
        const std::vector<vk::Image>& getImages() const { return m_images; }
        const std::vector<vk::raii::ImageView>& getImageViews() const { return m_imageViews; }
        const std::vector<vk::raii::Framebuffer>& getFramebuffers() const { return m_framebuffers; }
        


    private:
        void createSwapChain(const VulkanContext& context, const Window& window);
        void createImageViews(const vk::raii::Device& device);
        void cleanup();
        static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
        static vk::SurfaceFormatKHR pickSwapSurfaceFormat(const vk::PhysicalDevice device, const vk::SurfaceKHR surface);
        static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, vk::PresentModeKHR preferredMode);
        static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const Window& window);
        SwapChainSupportDetails m_supportDetails;
        vk::SurfaceFormatKHR m_surfaceFormat;
        vk::PresentModeKHR m_presentMode;
        vk::Extent2D m_extent;
        vk::raii::SwapchainKHR m_swapChain;
        std::vector<vk::Image> m_images;
        std::vector<vk::raii::ImageView> m_imageViews;
        std::vector<vk::raii::Framebuffer> m_framebuffers;
    };


}