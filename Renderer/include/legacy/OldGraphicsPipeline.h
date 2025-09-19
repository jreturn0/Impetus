//#pragma once
//#include "utils/VKCommon.hpp"
//#include "utils/QuickMacros.h"
//#include "utils/shader/ShaderCompiler.h"
//#include "utils/PipelineBuilder.h"
//#include "utils/Forwards.h"
//
//namespace imp::gfx {
//
//	using UniqueGraphicsPipeline = std::unique_ptr<class GraphicsPipeline>;
//	class GraphicsPipeline
//	{
//	public:
//		GraphicsPipeline()=default;
//		GraphicsPipeline(const vk::raii::Device& device, std::vector<vk::DescriptorSetLayout> descriptorSetLayouts,
//		                 vk::PushConstantRange pushRange, const std::string& shader, vk::PrimitiveTopology topology,
//		                 vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace,
//		                 vk::Format colorFormat, vk::Format depthFormat, bool enableDepthTest, bool enableBlending,
//		                 bool enableMultisampling);
//		//GraphicsPipeline(const vk::raii::Device& device, PipelineBuilder& builder, vk::PipelineLayout layout);
//
//		~GraphicsPipeline();
//		GraphicsPipeline(const GraphicsPipeline&) = delete; 
//		GraphicsPipeline& operator=(const GraphicsPipeline&) = delete; 
//		GraphicsPipeline(GraphicsPipeline&&) = delete; 
//		GraphicsPipeline& operator=(GraphicsPipeline&&) = delete;
//
//
//		vk::PipelineLayout getLayout() { return *m_layout; }
//		vk::Pipeline getPipeline() { return *m_pipeline; }
//
//
//		void bind( vk::CommandBuffer cmd) const;
//	private:
//
//		vk::raii::PipelineLayout m_layout{ nullptr };
//		vk::raii::Pipeline m_pipeline{ nullptr };
//		PipelineBuilder m_builder;
//
//	};
//	
//
//}
