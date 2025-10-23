#pragma once

#include "SwapChainSupportDetails.h"
#include <core/VulkanContext.h>

namespace imp::gfx
{
    // Manages the Vulkan Swap Chain and its associated resources
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



        // Getters
        inline const SwapChainSupportDetails& getDetails() const noexcept { return m_supportDetails; }
        inline vk::SurfaceFormatKHR getSurfaceFormat() const noexcept  { return m_surfaceFormat; }
        inline vk::PresentModeKHR getPresentMode() const noexcept { return m_presentMode; }
        inline vk::Extent2D getExtent() const noexcept { return m_extent; }
        inline const vk::raii::SwapchainKHR& getSwapChain() const noexcept { return m_swapChain; }
        inline const std::vector<vk::Image>& getImages() const noexcept { return m_images; }
        inline const std::vector<vk::raii::ImageView>& getImageViews() const noexcept { return m_imageViews; }
        inline const std::vector<vk::raii::Framebuffer>& getFramebuffers() const noexcept { return m_framebuffers; }
        inline vk::Semaphore getPresentCompleteSemaphore(size_t index) const noexcept { return m_presentCompleteSemaphores[index]; }
        inline vk::Semaphore getRenderFinishedSemaphore(size_t index) const noexcept { return m_renderFinishedSemaphores[index]; }
        inline size_t getImageCount() const noexcept { return m_images.size(); }
               
        // Public Methods

        void recreateSwapChain(const VulkanContext& context, const Window& window);

    private:
        SwapChainSupportDetails m_supportDetails{};
        vk::SurfaceFormatKHR m_surfaceFormat{};
        vk::PresentModeKHR m_presentMode{};
        vk::Extent2D m_extent{};
        vk::raii::SwapchainKHR m_swapChain{ nullptr };
        std::vector<vk::Image> m_images{};
        std::vector<vk::raii::ImageView> m_imageViews{};
        std::vector<vk::raii::Framebuffer> m_framebuffers{};
        std::vector<vk::raii::Semaphore> m_presentCompleteSemaphores{};
        std::vector<vk::raii::Semaphore> m_renderFinishedSemaphores{};

        // Private Methods

        void createSwapChain(const VulkanContext& context, const Window& window);
        void createImageViews(const vk::raii::Device& device);
        void cleanup();

        // Static Helper Methods

        static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
        static vk::SurfaceFormatKHR pickSwapSurfaceFormat(const vk::PhysicalDevice device, const vk::SurfaceKHR surface);
        static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes, vk::PresentModeKHR preferredMode);
        static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const Window& window);

    };


}