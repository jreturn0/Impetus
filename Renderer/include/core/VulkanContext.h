#pragma once
#include "ImmediateCommands.h"
#include "QueueFamilyIndices.h"
#include "utils/VKCommon.hpp"
#include "VmaUsage.h"
#include "window/Window.h"


namespace imp::gfx {
    // Manages core Vulkan objects and resources
    class VulkanContext
    {
    public:
        VulkanContext() = default;
        VulkanContext(Window& window);
        ~VulkanContext();
        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;
        VulkanContext(VulkanContext&&) = delete;
        VulkanContext& operator=(VulkanContext&&) = delete;

        // Getters

        inline const vk::raii::Instance& getInstance() const noexcept { return m_instance; };
        inline const vk::raii::PhysicalDevice& getPhysicalDevice() const noexcept { return m_physicalDevice; }
        inline const vk::raii::Device& getDevice() const noexcept { return m_device; }
        inline const vk::raii::SurfaceKHR& getSurface() const noexcept { return m_surface; }
        inline const QueueFamilyIndices& getQueueFamilyIndices() const noexcept { return m_queueFamilyIndices; }
        inline const vk::raii::Queue& getGraphicsQueue() const noexcept { return m_graphicsQueue; }
        inline VmaAllocator& getVmaAllocator()  noexcept { return m_vmaAllocator; }
        inline const VmaAllocator& getVmaAllocator() const noexcept { return m_vmaAllocator; }
        inline std::set<uint32_t> getUniqueQueueFamilies() const { return { m_queueFamilyIndices.graphicsFamily.value(), m_queueFamilyIndices.presentFamily.value() }; }
        inline const vk::raii::CommandPool& getGraphicsCommandPool() const noexcept { return m_graphicsCommandPool; }
        inline const ImmediateCommands& getGraphicsImmediateCommands() const noexcept { return m_graphicsImmediateCommands; }
    private:
        vk::raii::Context m_context{};
        vk::raii::Instance m_instance{ nullptr };
        vk::raii::DebugUtilsMessengerEXT m_debugUtilsMessenger{ nullptr };
        vk::raii::SurfaceKHR m_surface{ nullptr };
        vk::raii::PhysicalDevice m_physicalDevice{ nullptr };
        QueueFamilyIndices m_queueFamilyIndices{};
        vk::raii::Device m_device{ nullptr };
        VmaAllocator m_vmaAllocator{};
        vk::raii::Queue m_graphicsQueue{ nullptr };
        vk::raii::CommandPool m_graphicsCommandPool{ nullptr };
        ImmediateCommands m_graphicsImmediateCommands{};
    };
    // Factory function to create a unique pointer to VulkanContext ( handles exceptions )
    std::unique_ptr<VulkanContext> CreateVulkanContext(Window& window);

}