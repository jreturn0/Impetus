#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class PipelineBuilder
	{
	private:

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
		vk::PipelineRasterizationStateCreateInfo rasterizer;
		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		vk::PipelineMultisampleStateCreateInfo multisampling;
		vk::PipelineDepthStencilStateCreateInfo depthStencil;
		vk::PipelineRenderingCreateInfoKHR renderInfo;
		vk::Format colorFormat;
	



	public:

		PipelineBuilder();

		void clear();

		vk::UniquePipeline buildPipeline(const class Device& device, const vk::PipelineLayout& layout);
		void setShaderStages(const vk::ShaderModule& vertex, const vk::ShaderModule& fragment);
		void setInputTopology(vk::PrimitiveTopology topology);
		void setPolygonMode(vk::PolygonMode mode);
		void setCullMode(vk::CullModeFlags mode,vk::FrontFace frontFace);
		void setMultisamplingNone();
		void disableBlending();
		void enableBlendingAdditive();
		void enableBlendingAlphaBlend();
		void setColorAttachmentFormat(vk::Format format);
		void setDepthFormat(vk::Format format);
		void disableDepthTest();
		void enableDepthTest(bool depthWriteEnable, vk::CompareOp op);
		DISABLE_COPY_AND_MOVE(PipelineBuilder);
		
	};
}
