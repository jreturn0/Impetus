#include "pch.h"
#include "utils/GUI.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "core/CommandBuffer.h"
#include "core/Device.h"
#include "core/Image.h"
#include "core/Instance.h"
#include "core/Queue.h"
#include "core/SwapChain.h"
#include "window/VKWindow.h"



Imp::Render::GUI::GUI(Instance& instance, Device& device, SwapChain& swapChain, Queue& graphicsQueue, VKWindow& window)
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

	vk::DescriptorPoolCreateInfo pool_info (vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,1000,poolSizes);
	 pool = device.getLogical().createDescriptorPoolUnique(pool_info);


	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance.getInstance();
	init_info.PhysicalDevice = device.getPhysical();
	init_info.Device = device.getLogical();
	init_info. QueueFamily = device.getIndices().graphicsFamily.value();
	init_info.Queue = graphicsQueue.getQueue();
	init_info.DescriptorPool = *pool;
	init_info.MinImageCount = 3;// swapChain->getImageCount();
	init_info.ImageCount = 3;// swapChain->getImageCount();
	init_info.UseDynamicRendering = true;

	init_info.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	VkFormat format = static_cast<VkFormat>(swapChain.getFormat());
	init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &format;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForVulkan(window.getGLFW(), true);
	ImGui_ImplVulkan_Init(&init_info);
	ImGui_ImplVulkan_CreateFontsTexture();


}

Imp::Render::GUI::~GUI()
{
	ImGui_ImplVulkan_Shutdown();
}

void Imp::Render::GUI::startFrame()
{
	
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	
	
}

void Imp::Render::GUI::render( const vk::CommandBuffer& cmd)
{

	// Render ImGui draw data
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

}





void Imp::Render::GUI::endFrame()
{
	ImGui::Render();
	
}
