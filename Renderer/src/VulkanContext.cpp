
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <map>
#include <unordered_set>
//#include <WinUser.h>
#include <sstream>
#include <format>
#include <glm/vec3.hpp>
#include "core/VulkanContext.h"
#include "core/QueueFamilyIndices.h"
#include "core/SwapChainSupportDetails.h"
#include "ConfigSystem.h"
#include <utils/VKUtils.h>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace {


    bool CheckExtensionsAvailability(const std::vector<const char*>& requiredExtensions)
    {
        auto availableExtensions = vk::enumerateInstanceExtensionProperties();
        std::set<std::string> availableExtensionsSet;
        for (const auto& extension : availableExtensions) {
            availableExtensionsSet.insert(extension.extensionName);
        }

        for (const auto& required : requiredExtensions) {
            if (!availableExtensionsSet.contains(required)) {
                Debug::Error("Required extension not available: {}", required);
                return false;
            }
        }
        return true;
    }

    bool CheckValidationLayerSupport()
    {
        const std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
        for (const char* layerName : imp::gfx::vkutil::VALIDATION_LAYERS) {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) {
                Debug::Error("Validation layer not available: {}", layerName);
                return false;
            }
        }
        return true;
    }

    std::vector<const char*> GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (imp::gfx::vkutil::ENABLE_VALIDATION_LAYERS) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }
        extensions.insert(extensions.end(), imp::gfx::vkutil::INSTANCE_EXTENSIONS.begin(), imp::gfx::vkutil::INSTANCE_EXTENSIONS.end());
        return extensions;
    }

    auto s_appName = utl::ConfigValueRef<std::string>("render.app_name", "Impetus Application");

    vk::raii::Instance CreateInstance(const vk::raii::Context& context)
    {
        std::vector<const char*> extensions = GetRequiredExtensions();

        if (!CheckExtensionsAvailability(extensions))
            throw std::runtime_error("Required extensions are not available");

        if (imp::gfx::vkutil::ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport())
            throw std::runtime_error("Validation layers are not available");



        const auto appInfo = vk::ApplicationInfo{}
            .setApiVersion(vk::ApiVersion14)
            .setApplicationVersion(vk::makeApiVersion(0, 1, 0, 0))
            .setEngineVersion(vk::makeApiVersion(0, 1, 0, 0))
            .setPApplicationName(s_appName.get().c_str())
            .setPEngineName("Impetus");


        auto validationLayers = imp::gfx::vkutil::ENABLE_VALIDATION_LAYERS ?
            imp::gfx::vkutil::VALIDATION_LAYERS : std::vector<const char*>{};
        vk::InstanceCreateInfo createInfo{};
        createInfo.setPApplicationInfo(&appInfo)
            .setPEnabledExtensionNames(extensions)
            .setPEnabledLayerNames(validationLayers);


        vk::raii::Instance instance(context, createInfo);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance); // Initialize default dispatcher
        return instance;
    }





    VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageFunc(vk::DebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT              messageTypes,
        vk::DebugUtilsMessengerCallbackDataEXT const* pCallbackData,
        void* /*pUserData*/)
    {



        std::string message = std::format(
            "{}: {}:\n"
            "\tmessageIDName   = <{}>\n"
            "\tmessageIdNumber = {}\n"
            "\tmessage         = <{}>\n",
            vk::to_string(messageSeverity),
            vk::to_string(messageTypes),
            pCallbackData->pMessageIdName,
            pCallbackData->messageIdNumber,
            pCallbackData->pMessage
        );

        if (pCallbackData->queueLabelCount > 0) {
            message += "\tQueue Labels:\n";
            for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
                message += std::format("\t\tlabelName = <{}>\n", pCallbackData->pQueueLabels[i].pLabelName);
            }
        }

        if (pCallbackData->cmdBufLabelCount > 0) {
            message += "\tCommandBuffer Labels:\n";
            for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; ++i) {
                message += std::format("\t\tlabelName = <{}>\n", pCallbackData->pCmdBufLabels[i].pLabelName);
            }
        }

        if (pCallbackData->objectCount > 0) {
            message += "\tObjects:\n";
            for (uint32_t i = 0; i < pCallbackData->objectCount; ++i) {
                message += std::format("\t\tObject {}\n", i);
                message += std::format("\t\t\tobjectType   = {}\n", vk::to_string(pCallbackData->pObjects[i].objectType));
                message += std::format("\t\t\tobjectHandle = {}\n", pCallbackData->pObjects[i].objectHandle);
                if (pCallbackData->pObjects[i].pObjectName) {
                    message += std::format("\t\t\tobjectName   = <{}>\n", pCallbackData->pObjects[i].pObjectName);
                }
            }
        }

        //#ifdef _WIN32
        //        MessageBoxA(NULL, message.c_str(), "Alert", MB_OK);
        //#else
        Debug::Info("{}", message);
        //#endif
        if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
            Debug::Info("{}", "uh oh, error");
        }
        return VK_FALSE;
    }




    vk::DebugUtilsMessengerCreateInfoEXT CreateDebugMessengerCreateInfo() {
        return vk::DebugUtilsMessengerCreateInfoEXT{
            {},
            imp::gfx::vkutil::DEBUG_MESSAGE_SEVERITY,
            imp::gfx::vkutil::DEBUG_MESSAGE_TYPE,
            reinterpret_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(DebugMessageFunc),
            nullptr
        };
    }




    vk::raii::SurfaceKHR CreateSurface(const vk::raii::Instance& instance, const imp::Window& window) {
        VkSurfaceKHR _surface;
        if (glfwCreateWindowSurface(*instance, window.getGLFW(), nullptr, &_surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
        return vk::raii::SurfaceKHR(instance, _surface);
    }




    bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& device)
    {
        //v1
        auto extProps = device.enumerateDeviceExtensionProperties();
        std::unordered_set<std::string_view> requiredExtensions(
            imp::gfx::vkutil::DEVICE_EXTENSIONS.begin(),
            imp::gfx::vkutil::DEVICE_EXTENSIONS.end()
        );

        for (const auto& extension : extProps) {
            requiredExtensions.erase(extension.extensionName);
            if (requiredExtensions.empty()) break;
        }

        return requiredExtensions.empty();
    }

    [[nodiscard]] vk::raii::PhysicalDevice PickPhysicalDevice(const vk::raii::Instance& instance, const vk::raii::SurfaceKHR& surface) {
        auto devices = vk::raii::PhysicalDevices(instance);




        std::multimap<int, vk::raii::PhysicalDevice> candidates;

        for (auto& device : devices) {
            auto properties = device.getProperties();
            auto features = device.getFeatures();

            // Check required device extensions
            auto extProps = device.enumerateDeviceExtensionProperties();
            std::unordered_set<std::string_view> requiredExtensions(
                imp::gfx::vkutil::DEVICE_EXTENSIONS.begin(),
                imp::gfx::vkutil::DEVICE_EXTENSIONS.end()
            );

            for (const auto& extension : extProps) {
                requiredExtensions.erase(extension.extensionName);
                if (requiredExtensions.empty()) break;
            }

            auto extensionsSupported = requiredExtensions.empty();
            if (!extensionsSupported) continue;

            // Find queue families
            auto indices = imp::gfx::vkutil::FindQueueFamilies(device, surface);
            if (!indices.isComplete()) continue;


            //  required features
            if (!features.geometryShader || !features.samplerAnisotropy) continue;

            // Score device
            uint32_t score = 0;
            if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 1000;
            score += properties.limits.maxImageDimension2D;
            score += static_cast<uint32_t>(imp::gfx::vkutil::GetMaxUsableSampleCount(device));

            candidates.insert({ score,  std::move(device) });
        }

        if (candidates.empty()) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        if (candidates.rbegin()->first > 0) {
            return { candidates.rbegin()->second };
        }
        else {
            throw std::runtime_error("failed to find a suitable GPU!");
        }


    }



    vk::raii::Device CreateDevice(const vk::raii::PhysicalDevice& physicalDevice, const imp::gfx::QueueFamilyIndices& indices) {
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { *indices.graphicsFamily, *indices.presentFamily };
        if (indices.transferFamily.has_value()) uniqueQueueFamilies.insert(*indices.transferFamily);
        static constexpr float queuePriority = 1.0f;
        queueCreateInfos.reserve(uniqueQueueFamilies.size());
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{
                {},
                queueFamily,
                1,
                &queuePriority
                });
        }

        auto supportedFeatures = physicalDevice.getFeatures2<
            vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan12Features,
            vk::PhysicalDeviceVulkan13Features>();

        const auto& supported = supportedFeatures.get<vk::PhysicalDeviceFeatures2>().features;
        const auto& supported12 = supportedFeatures.get<vk::PhysicalDeviceVulkan12Features>();
        const auto& supported13 = supportedFeatures.get<vk::PhysicalDeviceVulkan13Features>();

        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT enabledFeatures14(true);

        if (!supported13.dynamicRendering)   Debug::Throw("Dynamic rendering feature is not supported.");
        if (!supported13.synchronization2)   Debug::Throw("Synchronization2 feature is not supported.");
        if (!supported12.bufferDeviceAddress) Debug::Throw("Buffer device address feature is not supported by the device.");

        auto enabledFeatures13 = vk::PhysicalDeviceVulkan13Features{}
            .setDynamicRendering(vk::True)
            .setSynchronization2(vk::True)
            .setPNext(&enabledFeatures14);

        auto enabledFeatures12 = vk::PhysicalDeviceVulkan12Features{}
        .setBufferDeviceAddress(vk::True)
            .setPNext(enabledFeatures13);

        auto enabledFeatures = vk::PhysicalDeviceFeatures2{}
            .setFeatures(vk::PhysicalDeviceFeatures{}
                .setFillModeNonSolid(supported.fillModeNonSolid)
                .setWideLines(supported.wideLines)
                .setSamplerAnisotropy(supported.samplerAnisotropy)
                .setSampleRateShading(supported.sampleRateShading))
            .setPNext(&enabledFeatures12);

        const auto deviceCreateInfo = vk::DeviceCreateInfo{}
            .setQueueCreateInfos(queueCreateInfos)
            .setPEnabledExtensionNames(imp::gfx::vkutil::DEVICE_EXTENSIONS)
            .setPNext(&enabledFeatures);

        auto device = vk::raii::Device(physicalDevice, deviceCreateInfo);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*device);
        return device;
    }

    VmaAllocator CreateVmaAllocator(const vk::raii::Instance& instance, const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::Device& device) {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = *physicalDevice;
        allocatorInfo.device = *device;
        allocatorInfo.instance = *instance;
        allocatorInfo.vulkanApiVersion = vk::ApiVersion14;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        VmaAllocator vma;
        if (vmaCreateAllocator(&allocatorInfo, &vma) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create VMA allocator");
        }
        return vma;
    }


}

imp::gfx::VulkanContext::VulkanContext(Window& window) :
    m_context(),
    m_instance(CreateInstance(m_context)),
    m_debugUtilsMessenger(m_instance, CreateDebugMessengerCreateInfo()),
    m_surface(CreateSurface(m_instance, window)),
    m_physicalDevice(PickPhysicalDevice(m_instance, m_surface)),
    m_queueFamilyIndices(vkutil::FindQueueFamilies2(m_physicalDevice, m_surface)),
    m_device(CreateDevice(m_physicalDevice, m_queueFamilyIndices)),
    m_vmaAllocator(CreateVmaAllocator(m_instance, m_physicalDevice, m_device)),
    m_graphicsQueue(m_device.getQueue(*m_queueFamilyIndices.graphicsFamily, 0)),
    m_graphicsCommandPool(m_device, vk::CommandPoolCreateInfo{ vk::CommandPoolCreateFlagBits::eResetCommandBuffer, *m_queueFamilyIndices.graphicsFamily }),
    m_graphicsImmediateCommands(m_device, *m_queueFamilyIndices.graphicsFamily)

{
}

imp::gfx::VulkanContext::~VulkanContext()
{
    m_device.waitIdle();

    if (m_vmaAllocator) {
        vmaDestroyAllocator(m_vmaAllocator);
        m_vmaAllocator = nullptr;
    }
    m_device.waitIdle();
}

std::unique_ptr<imp::gfx::VulkanContext> imp::gfx::CreateVulkanContext(Window& window)
{
    try {
        return std::make_unique<VulkanContext>(window);
    }
    catch (const vk::SystemError& err) {
        Debug::Error("Failed to create Vulkan context: {}", err.what());
    }
    catch (const std::exception& e) {
        Debug::Error("Failed to create Vulkan context: {}", e.what());
    }
    return nullptr;
}
