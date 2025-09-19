#pragma once
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <fmt/core.h>
#include <source_location>
#include <set>
#include <span>
#include <array>
#include "QuickMacros.h"
#include "VkConstants.h"
#include <map>
#include <stdexcept>  
#include "Debug.h"
#include "StringHash.h"


namespace imp::gfx::vkutil {
    constexpr uint64_t ERROR_IMAGE_HASH = utl::StringHash("error_image");
    constexpr uint64_t WHITE_IMAGE_HASH = utl::StringHash("white_image");
    constexpr uint64_t GREY_IMAGE_HASH = utl::StringHash("grey_image");
    constexpr uint64_t BLACK_IMAGE_HASH = utl::StringHash("black_image");
    constexpr int32_t MAX_FRAMES_IN_FLIGHT = 2;
    constexpr uint32_t MIN_IMAGE_COUNT = 3;

    enum class ShaderStage
    {
        None = INT32_MAX,
        Vertex = 0,
        Fragment = 1,
        Compute = 2,
        Geometry = 3
    };
#ifdef NDEBUG
    constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
    constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

    const std::vector<const char*> VALIDATION_LAYERS = {
           "VK_LAYER_KHRONOS_validation"
    };
    const std::vector INSTANCE_EXTENSIONS = {
        vk::EXTSwapchainColorSpaceExtensionName,
        vk::EXTDebugUtilsExtensionName
    };

    const std::vector<const char*> DEVICE_EXTENSIONS = {
        vk::KHRSwapchainExtensionName,
        vk::KHRSpirv14ExtensionName,
        vk::KHRSynchronization2ExtensionName,
        vk::KHRDynamicRenderingExtensionName,
        vk::KHRBufferDeviceAddressExtensionName,
        vk::KHRCreateRenderpass2ExtensionName
    };

    constexpr vk::DebugUtilsMessageSeverityFlagsEXT DEBUG_MESSAGE_SEVERITY =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    constexpr vk::DebugUtilsMessageTypeFlagsEXT DEBUG_MESSAGE_TYPE =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;


    inline void CheckResult(const vk::Result result, const std::source_location& src = std::source_location::current())
    {
        Debug::AssertThrow(result == vk::Result::eSuccess, "Vulkan error encountered: VKResult is \"{}\" in {} at line {}", vk::to_string(result), src.file_name(), src.line());
    }
}
