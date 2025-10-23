#pragma once
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"
#include <optional>
namespace imp::gfx {
    // Structure to hold queue family indices
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> transferFamily; // Currently unused, transfer operations use graphics queue. 

        // Check if all required queue families are found
        inline bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value(); }
        // Check if only graphics and present families are found
        inline bool isComplete2() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };

    namespace vkutil
    {
        // Find queue family indices for a given physical device and surface
        QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
        // Find queue family indices for a given physical device and surface (only graphics and present)
        QueueFamilyIndices FindQueueFamilies2(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
    }
}
    