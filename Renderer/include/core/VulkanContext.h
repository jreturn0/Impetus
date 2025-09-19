#pragma once
#include "utils/VKCommon.hpp"
#include "QueueFamilyIndices.h"
#include "VmaUsage.h"
#include "ImmediateCommands.h"
#include "window/Window.h"


namespace imp::gfx {
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




        const vk::raii::Instance& getInstance() const { return m_instance; };
        const vk::raii::PhysicalDevice& getPhysicalDevice() const { return m_physicalDevice; }
        const vk::raii::Device& getDevice() const { return m_device; }
        const vk::raii::SurfaceKHR& getSurface() const { return m_surface; }
        const QueueFamilyIndices& getQueueFamilyIndices() const { return m_queueFamilyIndices; }
        //const VmaAllocator& getVmaAllocator() const { return m_vmaAllocator; }
        const vk::raii::Queue& getGraphicsQueue() const { return m_graphicsQueue; }
        VmaAllocator& getVmaAllocator() { return m_vmaAllocator; }
        const VmaAllocator& getVmaAllocator() const { return m_vmaAllocator; }
        std::set<uint32_t> getUniqueQueueFamilies() const {return { m_queueFamilyIndices.graphicsFamily.value(), m_queueFamilyIndices.presentFamily.value() };}

        const vk::raii::CommandPool& getGraphicsCommandPool() const { return m_graphicsCommandPool; }
        const ImmediateCommands& getGraphicsImmediateCommands() const { return m_graphicsImmediateCommands; }


    private:


        vk::raii::Context m_context;
        vk::raii::Instance m_instance;
        vk::raii::DebugUtilsMessengerEXT m_debugUtilsMessenger;
        vk::raii::SurfaceKHR m_surface;
        vk::raii::PhysicalDevice m_physicalDevice;
        QueueFamilyIndices m_queueFamilyIndices;
        vk::raii::Device m_device;
        VmaAllocator m_vmaAllocator;
        vk::raii::Queue m_graphicsQueue;
        vk::raii::CommandPool m_graphicsCommandPool;
        ImmediateCommands m_graphicsImmediateCommands;


    };

    std::unique_ptr<VulkanContext> CreateVulkanContext(Window& window);

}