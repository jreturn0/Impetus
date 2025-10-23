#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "utils/GUI.h"
#include <core/VulkanContext.h>
#include <window/Window.h>





imp::gfx::GUI::GUI(VulkanContext& context, Window& window, vk::Format drawFormat)
{

    vk::DescriptorPoolSize poolSizes[] =
    {
        { vk::DescriptorType::eSampler, 1000 },
        { vk::DescriptorType::eCombinedImageSampler, 1000 },
        { vk::DescriptorType::eSampledImage, 1000 },
        { vk::DescriptorType::eStorageImage, 1000 },
        { vk::DescriptorType::eUniformTexelBuffer, 1000 },
        { vk::DescriptorType::eStorageTexelBuffer, 1000 },
        { vk::DescriptorType::eUniformBuffer, 1000 },
        { vk::DescriptorType::eStorageBuffer, 1000 },
        { vk::DescriptorType::eUniformBufferDynamic, 1000 },
        { vk::DescriptorType::eStorageBufferDynamic, 1000 },
        { vk::DescriptorType::eInputAttachment, 1000 }
    };
    auto& device = context.getDevice();


    vk::DescriptorPoolCreateInfo pool_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, 1000, poolSizes);
    m_descriptorPool = device.createDescriptorPool(pool_info);


    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = *context.getInstance();
    init_info.PhysicalDevice = *context.getPhysicalDevice();
    init_info.Device = *context.getDevice();
    init_info.QueueFamily = *context.getQueueFamilyIndices().graphicsFamily;
    init_info.Queue = *context.getGraphicsQueue();
    init_info.DescriptorPool = *m_descriptorPool;
    init_info.MinImageCount = 3;// swapChain->getImageCount();
    init_info.ImageCount = 3;// swapChain->getImageCount();
    init_info.UseDynamicRendering = true;

    init_info.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
    init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    VkFormat format = static_cast<VkFormat>(drawFormat);
    init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &format;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window.getGLFW(), true);
    ImGui_ImplVulkan_Init(&init_info);
    ImGui_ImplVulkan_CreateFontsTexture();


}

imp::gfx::GUI::~GUI()
{
    ImGui_ImplVulkan_Shutdown();
}

void imp::gfx::GUI::startFrame()
{

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();




}

void imp::gfx::GUI::render(vk::CommandBuffer cmd)
{

    // Render ImGui draw data
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

}





void imp::gfx::GUI::endFrame()
{
    ImGui::Render();

}
