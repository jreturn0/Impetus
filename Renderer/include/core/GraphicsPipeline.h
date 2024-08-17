#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "utils/shader/ShaderCompiler.h"
#include "utils/PipelineBuilder.h"
#include "utils/Forwards.h"

namespace Imp::Render {

	using UniqueGraphicsPipeline = std::unique_ptr<class GraphicsPipeline>;
	class GraphicsPipeline
	{
	private:
		
		vk::UniquePipelineLayout layout;
		vk::UniquePipeline pipeline;
		PipelineBuilder builder;


	public:


		GraphicsPipeline()=default;
		GraphicsPipeline(const Device& device, std::vector<vk::DescriptorSetLayout> descriptorSetLayouts,
		                 vk::PushConstantRange pushRange, const std::string& shader, vk::PrimitiveTopology topology,
		                 vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace,
		                 vk::Format colorFormat, vk::Format depthFormat, bool enableDepthTest, bool enableBlending,
		                 bool enableMultisampling);
		GraphicsPipeline(const Device& device, PipelineBuilder& builder, vk::PipelineLayout layout);

		~GraphicsPipeline();
		DISABLE_COPY_AND_MOVE(GraphicsPipeline);


		const vk::PipelineLayout& getLayout() { return *layout; }
		const vk::Pipeline& getPipeline() { return *pipeline; }
		operator vk::Pipeline& () { return *pipeline; }

		void bind(const vk::CommandBuffer& cmd) const;

		void recreate();
	};
	

}
