#pragma once
#include <unordered_map>

#include "LoadedGLTF.h"
#include "core/Image.h"
#include "descriptor/DescriptorAllocatorGrowable.h"
#include "geometry/Mesh.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"



namespace Imp::Render {
	class ResourceCache
	{
	private:
		DescriptorAllocatorGrowable materialDescriptorAllocator;


		constexpr static bool generateNames = true;

		std::unordered_map <std::string, SharedMesh> meshes;
		std::vector<std::string> meshNames;
		std::unordered_map <std::string, SharedImage> images;
		std::vector<std::string> imageNames;
		std::unordered_map <std::string, SharedMaterial> materials;
		std::vector<std::string> materialNames;
		std::unordered_map <std::string, std::shared_ptr<LoadedGLTF>> gltfs;
		std::vector<std::string> gltfNames;
		std::vector<vk::UniqueSampler> samplers;

	public:
		DISABLE_COPY_AND_MOVE(ResourceCache);

		auto&& getMaterialDescriptorAllocator() { return materialDescriptorAllocator; }

		ResourceCache(const vk::Device& device);


		void addMesh(const std::string& name, SharedMesh mesh);
		bool hasMesh(const std::string& name) const;
		SharedMesh getMesh(const std::string& name) const;
		void removeMesh(const std::string& name);
		auto&& getMeshNames() { return meshNames; }

		void addImage(const std::string& name, SharedImage image);
		bool hasImage(const std::string& name) const;
		SharedImage getImage(const std::string& name) const;
		void removeImage(const std::string& name);
		auto&& getImageNames() { return imageNames; }

		void addMaterial(const std::string& name, SharedMaterial material);
		bool hasMaterial(const std::string& name) const;
		SharedMaterial getMaterial(const std::string& name) const;
		SharedMaterial& getMaterialRef(const std::string& name) ;
		void removeMaterial(const std::string& name);
		auto&& getMaterialNames() { return materialNames; }

		void addGLTF(const std::string& name, const SharedGLTF& gltf);
		bool hasGLTF(const std::string& name) const;
		SharedGLTF getGLTF(const std::string& name) const;
		void removeGLTF(const std::string& name);
		auto&& getGLTFNames() { return gltfNames; }



	};
}
