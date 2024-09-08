#include "pch.h"
#include "utils/PipelineBuilder.h"

#include "core/Device.h"

void Imp::Render::PipelineBuilder::disableBlending()
{
	colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	colorBlendAttachment.setBlendEnable(vk::False);
}
void Imp::Render::PipelineBuilder::enableBlendingAdditive()
{
	colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	colorBlendAttachment.setBlendEnable(vk::True);
	colorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
	colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eOne);
	colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
	colorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
	colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
	colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

}
void Imp::Render::PipelineBuilder::enableBlendingAlphaBlend()
{
	colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
	colorBlendAttachment.setBlendEnable(vk::True);
	colorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
	colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
	colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
	colorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
	colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
	colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

}

void Imp::Render::PipelineBuilder::setColorAttachmentFormat(vk::Format format)
{
	colorFormat = format;
	renderInfo.setColorAttachmentFormats(colorFormat);
}

void Imp::Render::PipelineBuilder::setDepthFormat(vk::Format format)
{
	renderInfo.setDepthAttachmentFormat(format);
}


void Imp::Render::PipelineBuilder::disableDepthTest()
{
	depthStencil.setDepthTestEnable(vk::False);
	depthStencil.setDepthWriteEnable(vk::False);
	depthStencil.setDepthCompareOp(vk::CompareOp::eNever);
	depthStencil.setDepthBoundsTestEnable(vk::False);
	depthStencil.setStencilTestEnable(vk::False);
	depthStencil.setMinDepthBounds(0.f);
	depthStencil.setMaxDepthBounds(1.f);



}

void Imp::Render::PipelineBuilder::enableDepthTest(bool depthWriteEnable, vk::CompareOp op)
{


	depthStencil = vk::PipelineDepthStencilStateCreateInfo{}
		.setDepthTestEnable(vk::True)
		.setDepthWriteEnable(depthWriteEnable ? vk::True : vk::False)
		.setDepthCompareOp(op)
		.setDepthBoundsTestEnable(vk::False)
		.setStencilTestEnable(vk::False)
		.setMinDepthBounds(0.0f)
		.setMaxDepthBounds(1.0f);

}

Imp::Render::PipelineBuilder::PipelineBuilder() :  shaderStages({}), inputAssembly({}), rasterizer({}), colorBlendAttachment({}), multisampling({}), depthStencil({}), renderInfo({}), colorFormat(vk::Format::eUndefined)
{

}

void Imp::Render::PipelineBuilder::clear()
{
	shaderStages.clear();
	inputAssembly = vk::PipelineInputAssemblyStateCreateInfo{};
	rasterizer = vk::PipelineRasterizationStateCreateInfo{};
	colorBlendAttachment = vk::PipelineColorBlendAttachmentState{};
	multisampling = vk::PipelineMultisampleStateCreateInfo{};
	depthStencil = vk::PipelineDepthStencilStateCreateInfo{};
	renderInfo = vk::PipelineRenderingCreateInfo{};
	colorFormat = vk::Format::eUndefined;

}
vk::UniquePipeline Imp::Render::PipelineBuilder::	buildPipeline(const Device& device, const vk::PipelineLayout& layout)
{
	vk::PipelineViewportStateCreateInfo viewportState({}, 1, nullptr, 1, nullptr);
	vk::PipelineColorBlendStateCreateInfo colorBlending({}, vk::False, vk::LogicOp::eCopy, colorBlendAttachment);
	vk::PipelineVertexInputStateCreateInfo vertexInput{};
	vk::DynamicState dynamicStates[] = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicState({}, dynamicStates);
	//std::cout << "Color Format: " << vk::to_string(colorFormat) << std::endl;
	//std::cout << "Depth Format: " << static_cast<uint32_t>(depthFormat) << std::endl;
	
	vk::GraphicsPipelineCreateInfo pipelineInfo(
		{}, // Flags
		shaderStages,
		&vertexInput,
		&inputAssembly,
		nullptr, // Tessellation state
		&viewportState,
		&rasterizer,
		&multisampling,
		&depthStencil,
		&colorBlending,
		&dynamicState,
		layout,
		nullptr, // Render pass
		0, // Subpass
		nullptr, // Base pipeline handle
		-1, // Base pipeline index
		&renderInfo // Ensure renderInfo is correctly linked
	);

	auto result = device.getLogical().createGraphicsPipelineUnique({}, pipelineInfo);
	vkutil::CheckResult(result.result);

	return std::move(result.value);
}


void Imp::Render::PipelineBuilder::setShaderStages(const vk::ShaderModule& vertex, const vk::ShaderModule& fragment)
{
	auto vertexStage = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertex, "main");
	auto fragmentStage = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragment, "main");
	shaderStages.emplace_back(vertexStage);
	shaderStages.emplace_back(fragmentStage);
}

void Imp::Render::PipelineBuilder::setInputTopology(vk::PrimitiveTopology topology)
{
	inputAssembly.setTopology(topology);
	inputAssembly.setPrimitiveRestartEnable(vk::False); //vk::PipelineInputAssemblyStateCreateInfo({}, topology, vk::False);
}

void Imp::Render::PipelineBuilder::setPolygonMode(vk::PolygonMode mode)
{
	rasterizer.setPolygonMode(mode);
	rasterizer.setLineWidth(1.f);
}

void Imp::Render::PipelineBuilder::setCullMode(vk::CullModeFlags mode, vk::FrontFace frontFace)
{
	rasterizer.setCullMode(mode);
	rasterizer.setFrontFace(frontFace);
}

void Imp::Render::PipelineBuilder::setMultisamplingNone()
{
	multisampling.setSampleShadingEnable(vk::False);
	multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
	multisampling.setMinSampleShading(1.f);
	multisampling.setPSampleMask(nullptr);
	multisampling.setAlphaToCoverageEnable(vk::False);
	multisampling.setAlphaToOneEnable(vk::False);
}
