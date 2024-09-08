#include "pch.h"
#include "materials/MetallicRoughness.h"

#include "VKRenderer.h"
#include "utils/descriptor//DescriptorLayoutBuilder.h"
#include "core/Device.h"
#include "gpudata/GPUDrawPushConstants.h"
#include "core/GraphicsPipeline.h"
#include "utils/PipelineBuilder.h"
#include "utils/descriptor/DescriptorAllocatorGrowable.h"
#include "core/Buffer.h"
#include "core/Image.h"
#include "materials/Material.h"
Imp::Render::MetallicRoughness::MetallicRoughness(VKRenderer& renderer)
{
	buildPipelines(renderer);
}

void Imp::Render::MetallicRoughness::recreate(VKRenderer& renderer)
{
	
}

void Imp::Render::MetallicRoughness::buildPipelines(VKRenderer& renderer)
{
	vk::PushConstantRange matrixRange{ vk::ShaderStageFlagBits::eVertex,0 ,sizeof(GPUDrawPushConstants) };

	DescriptorLayoutBuilder layoutBuilder;
	layoutBuilder.addBinding(0, vk::DescriptorType::eUniformBuffer);
	layoutBuilder.addBinding(1, vk::DescriptorType::eCombinedImageSampler);
	layoutBuilder.addBinding(2, vk::DescriptorType::eCombinedImageSampler);

	materialLayout = layoutBuilder.build(renderer.getDevice().getLogical(),vk::ShaderStageFlagBits::eVertex| vk::ShaderStageFlagBits::eFragment);

	auto layouts = std::vector{
		renderer.getGPUSceneDataDescriptorLayout(),*materialLayout
	};
	







	opaque = std::make_unique<GraphicsPipeline>(renderer.getDevice(), layouts, matrixRange, "mesh_material",
	                                            vk::PrimitiveTopology::eTriangleList, vk::PolygonMode::eFill,
	                                            vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise,
	                                            renderer.getDrawImage().getFormat(),
	                                            renderer.getDepthImage().getFormat(),
	                                            true, false, true);
	transparent = std::make_unique<GraphicsPipeline>(renderer.getDevice(), layouts, matrixRange, "mesh_material",
	                                                 vk::PrimitiveTopology::eTriangleList, vk::PolygonMode::eFill,
	                                                 vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise,
	                                                 renderer.getDrawImage().getFormat(),
	                                                 renderer.getDepthImage().getFormat(),
	                                                 true, true, true);

}

void Imp::Render::MetallicRoughness::clear_resources(const vk::Device& device)
{
}

std::shared_ptr<Imp::Render::Material> Imp::Render::MetallicRoughness::write_material(const vk::Device& device,
	MaterialPass pass,  MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator)
{
	auto material = std::make_shared<Material>();
	material->passType = pass;
	if (pass == MaterialPass::Transparent)
	{
		material->pipeline = transparent.get();
	}
	else
	{
		material->pipeline = opaque.get();
	}
	
	material->set = descriptorAllocator.allocate(device, *materialLayout);
	
	writer.clear();
	writer.write_buffer(0, resources.dataBuffer->getBuffer(), sizeof(MaterialConstants), resources.dataOffset, vk::DescriptorType::eUniformBuffer);
	writer.write_image(1, resources.colorImage.lock()->getView(), *resources.colorSampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
	writer.write_image(2, resources.metalRoughImage.lock()->getView(), *resources.metalRoughSampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
	writer.update_set(device, *material->set);
	material->buffer = resources.dataBuffer;
	return material;
}

//
//void Imp::Render::MetallicRoughness::write_material(const vk::Device& device, MaterialPass pass,
//	const MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator)
//{
//	std::shared_ptr< Material> material;
//	material->passType = pass;
//	if (pass == MaterialPass::Transparent)
//	{
//		material->pipeline = transparent.get();
//	}
//	else
//	{
//		material->pipeline = opaque.get();
//	}
//	
//	material->set = descriptorAllocator.allocate(device,*materialLayout);
//	writer.clear();
//	writer.write_buffer(0, resources.dataBuffer->getBuffer(), sizeof(MaterialConstants), resources.dataOffset, vk::DescriptorType::eUniformBuffer);
//	writer.write_image(1, resources.colorImage->getView(), resources.getMetalRoughSampler(), vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
//	writer.write_image(2, resources.metalRoughImage->getView(), *resources.metalRoughSampler, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler);
//	writer.update_set(device, *material->set);
//
//	//return material;
//
//
//}


