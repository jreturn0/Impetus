#include "core/GraphicsPipeline.h"

#include <bitset>
#include "fstream"
#include "FileDirectoryHelper.h"
#include "core/Device.h"
#include "gpudata/GPUDrawPushConstants.h"
#include "core/SwapChain.h"

#include "geometry/Vertex.h"
#include "utils/shader/ShaderReflect.h"
#include "utils/shader/ShaderLoader.h"
#include "utils/PipelineBuilder.h"

Imp::Render::GraphicsPipeline::GraphicsPipeline(const Device& device,
												 std::vector<vk::DescriptorSetLayout> descriptorSetLayouts,
												vk::PushConstantRange pushRange,

												const std::string& shader,
												vk::PrimitiveTopology topology,
												vk::PolygonMode polygonMode,
												vk::CullModeFlags cullMode,
												vk::FrontFace frontFace,
												vk::Format colorFormat,
												vk::Format depthFormat,
												bool enableDepthTest,
												bool enableBlending,
												bool enableMultisampling)
{


	// Create the shader modules
	auto vertModule = ShaderLoader::CreateShaderModule(device, shader, vkutil::ShaderStage::Vertex);
	auto fragModule = ShaderLoader::CreateShaderModule(device, shader, vkutil::ShaderStage::Fragment);

	//vk::PushConstantRange pushRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(GPUDrawPushConstants));

	for( auto& layout : descriptorSetLayouts) {
		if (!layout) {
			
			throw std::runtime_error("Invalid descriptor set layout");
		}
		//std::cout << "Descriptor Set Layout\n";
		//std::cout << layout << "\n";
		
	}
	
	// Create the pipeline layout
	vk::PipelineLayoutCreateInfo layoutInfo({}, descriptorSetLayouts, pushRange);
	layout = device.getLogical().createPipelineLayoutUnique(layoutInfo);


	builder.setShaderStages(*vertModule, *fragModule);
	builder.setInputTopology(topology);
	builder.setPolygonMode(polygonMode);
	builder.setCullMode(cullMode, frontFace);
	if (!enableMultisampling) builder.setMultisamplingNone();
	if (!enableDepthTest) {
		builder.disableDepthTest();
		//std::cout << "Depth Disabled\n";
	} else {
		builder.enableDepthTest(true, vk::CompareOp::eLessOrEqual);
		//std::cout << "Depth Enabled\n";
	}
	if (!enableBlending) builder.disableBlending(); else builder.enableBlendingAdditive();


	builder.setColorAttachmentFormat(colorFormat);
	builder.setDepthFormat(depthFormat);
	pipeline = builder.buildPipeline(device,*layout);






}

Imp::Render::GraphicsPipeline::GraphicsPipeline(const Device& device, PipelineBuilder& builder, vk::PipelineLayout layout) : layout(layout)
{
	pipeline = builder.buildPipeline(device, layout);
}

Imp::Render::GraphicsPipeline::~GraphicsPipeline()
{}

void Imp::Render::GraphicsPipeline::bind(const vk::CommandBuffer& cmd) const
{
	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
}

void Imp::Render::GraphicsPipeline::recreate()
{
}

