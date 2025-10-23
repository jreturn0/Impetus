#include "core/QueueFamilyIndices.h"

imp::gfx::QueueFamilyIndices imp::gfx::vkutil::FindQueueFamilies(const vk::PhysicalDevice& device,
    const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices;

    auto queueFamilies = device.getQueueFamilyProperties();

    int count = 0;


    std::ranges::for_each(queueFamilies, [&](const vk::QueueFamilyProperties& queueFamily) {
        // Check for graphics support
        if ((queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) && !indices.graphicsFamily.has_value()) {
            indices.graphicsFamily = count;
        }
        // Check for present support
        if (device.getSurfaceSupportKHR(count, surface) && !indices.presentFamily.has_value()) {
            indices.presentFamily = count;
        };
        // Check for dedicated transfer queue
        if ((queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) &&
            !(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) &&
            !(queueFamily.queueFlags & vk::QueueFlagBits::eCompute) &&
            !indices.transferFamily.has_value()) {
            indices.transferFamily = count;
        }
        if (indices.isComplete()) {
            return;
        }
        count++;
        });

    // If no dedicated transfer, find first queue that supports transfer
    if (!indices.transferFamily.has_value()) {
        std::ranges::for_each(queueFamilies, [&](const vk::QueueFamilyProperties& queueFamily) {
            if ((queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)) {
                indices.transferFamily = count;
                return;
            }
            });

    }
    return indices;
}

imp::gfx::QueueFamilyIndices imp::gfx::vkutil::FindQueueFamilies2(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices;
    // find the index of the first queue family that supports graphics
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device.getQueueFamilyProperties();

    // get the first index into queueFamilyProperties which supports graphics
    auto graphicsQueueFamilyProperty = std::ranges::find_if(queueFamilyProperties, [](auto const& qfp)
        { return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0); });

    auto graphicsIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
    indices.graphicsFamily = graphicsIndex;
    // determine a queueFamilyIndex that supports present
    // first check if the graphicsIndex is good enough
    auto presentIndex = device.getSurfaceSupportKHR(graphicsIndex, surface)
        ? graphicsIndex
        : static_cast<uint32_t>(queueFamilyProperties.size());
    if (presentIndex == queueFamilyProperties.size())
    {
        // the graphicsIndex doesn't support present -> look for another family index that supports both
        // graphics and present
        for (size_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                device.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface))
            {
                graphicsIndex = static_cast<uint32_t>(i);
                presentIndex = graphicsIndex;
                break;
            }
        }
        if (presentIndex == queueFamilyProperties.size())
        {
            // there's nothing like a single family index that supports both graphics and present -> look for another
            // family index that supports present
            for (size_t i = 0; i < queueFamilyProperties.size(); i++)
            {
                if (device.getSurfaceSupportKHR(static_cast<uint32_t>(i), surface))
                {
                    presentIndex = static_cast<uint32_t>(i);
                    break;
                }
            }
        }
    }
    indices.presentFamily = presentIndex;
    if ((graphicsIndex == queueFamilyProperties.size()) || (presentIndex == queueFamilyProperties.size()))
    {
        throw std::runtime_error("Could not find a queue for graphics or present -> terminating");
    }

    return indices;
}
