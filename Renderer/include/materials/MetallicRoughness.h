#pragma once
#include <glm/vec4.hpp>

#include "core/Buffer.h"
#include "Material.h"
#include "utils/descriptor/DescriptorWriter.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"
#include "utils/Forwards.h"
#include "utils/descriptor/DescriptorWriter.h"

namespace Imp::Render
{

	struct MetallicRoughness
	{
		std::unique_ptr< GraphicsPipeline> opaque;
		std::unique_ptr< GraphicsPipeline> transparent;
		vk::UniqueDescriptorSetLayout materialLayout;

		struct MaterialConstants
		{
			glm::vec4 colorFactors;
			glm::vec4 metalRoughFactors;
			glm::vec4 extra[14];
		};

		struct MaterialResources
		{
			std::weak_ptr<Image> colorImage;
			vk::Sampler* colorSampler;
			std::weak_ptr < Image> metalRoughImage;
			vk::Sampler* metalRoughSampler;
			std::shared_ptr<Buffer> dataBuffer;
			uint32_t dataOffset;

			const auto& getMetalRoughSampler() const { return *metalRoughSampler; }

		};

		DescriptorWriter writer;
		MetallicRoughness( VKRenderer& renderer);
		void recreate( VKRenderer& renderer);
		void buildPipelines( VKRenderer& renderer);
		void clear_resources(const vk::Device& device);
		auto write_material(const vk::Device& device, MaterialPass pass,
		                    MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator) -> std::shared_ptr<Material>;
		DISABLE_COPY_AND_MOVE(MetallicRoughness);
		
	};
}
