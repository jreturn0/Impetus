#pragma once
#include <glm/fwd.hpp>
#include  "utils/VKCommon.hpp"
#include "MaterialPass.h";

namespace Imp::Render {


	struct Material
	{
		DISABLE_COPY_AND_MOVE(Material);
		std::string name;
		class GraphicsPipeline* pipeline;
		vk::UniqueDescriptorSet set;
		MaterialPass passType;
		std::shared_ptr<class Buffer> buffer;
		Material(GraphicsPipeline* pipeline, vk::UniqueDescriptorSet set, MaterialPass passType, std::shared_ptr<class Buffer> buffer) : pipeline(pipeline), set(std::move(set)), passType(passType), buffer(
			std::move(buffer))
		{}
		Material()
		{
			pipeline = nullptr;
			passType = MaterialPass::MainColor;
		}
	};
	using SharedMaterial = std::shared_ptr<Material>;




}
