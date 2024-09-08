#include "pch.h"
#include "utils/ResourceCache.h"


Imp::Render::ResourceCache::ResourceCache(const vk::Device& device)
{
	std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = { { vk::DescriptorType::eCombinedImageSampler, 3 },
		{ vk::DescriptorType::eUniformBuffer, 3 },
		{ vk::DescriptorType::eStorageBuffer, 1 } };

	materialDescriptorAllocator.init_pool(device, 8, sizes);
	meshNames.emplace_back("");
	materialNames.emplace_back("default");
}

void Imp::Render::ResourceCache::addMesh(const std::string& name, SharedMesh mesh)
{
	meshes[name] = mesh;
	if constexpr (generateNames) {
		meshNames.push_back(name);
	}
	//std::cout << "Added mesh: " << name << std::endl;
}

bool Imp::Render::ResourceCache::hasMesh(const std::string& name) const
{
	return meshes.contains(name);
}

Imp::Render::SharedMesh Imp::Render::ResourceCache::getMesh(const std::string& name) const
{
	return meshes.at(name);
}

void Imp::Render::ResourceCache::removeMesh(const std::string& name)
{
	std::erase(meshNames, name);
	meshes.erase(name);
}

void Imp::Render::ResourceCache::addImage(const std::string& name, SharedImage image)
{
	imageNames.push_back(name);
	images[name] = image;
	//std::cout << "Added image: " << name << std::endl;
}
bool Imp::Render::ResourceCache::hasImage(const std::string& name) const
{
	return images.contains(name);
}

Imp::Render::SharedImage Imp::Render::ResourceCache::getImage(const std::string& name) const
{
	return images.at(name);
}

void Imp::Render::ResourceCache::removeImage(const std::string& name)
{
	images.erase(name);
	std::erase(imageNames, name);
}



void Imp::Render::ResourceCache::addMaterial(const std::string& name, SharedMaterial material)
{
	materials[name] = material;
	materialNames.push_back(name);
	//std::cout << "Added material: " << name << std::endl;
}

bool Imp::Render::ResourceCache::hasMaterial(const std::string& name) const
{
	return materials.contains(name);
}

Imp::Render::SharedMaterial Imp::Render::ResourceCache::getMaterial(const std::string& name) const
{
	return materials.at(name);
}

Imp::Render::SharedMaterial& Imp::Render::ResourceCache::getMaterialRef(const std::string& name) 
{
	return materials.at(name);
}


void Imp::Render::ResourceCache::removeMaterial(const std::string& name)
{
	std::erase(materialNames, name);
	materials.erase(name);
}

void Imp::Render::ResourceCache::addGLTF(const std::string& name, const SharedGLTF& gltf)
{
	gltfs[name] = gltf;
	gltfNames.push_back(name);
	//std::cout << "Added gltf: " << name << std::endl;
}

bool Imp::Render::ResourceCache::hasGLTF(const std::string& name) const
{
	return gltfs.contains(name);

}

Imp::Render::SharedGLTF Imp::Render::ResourceCache::getGLTF(const std::string& name) const
{
	return gltfs.at(name);
}

void Imp::Render::ResourceCache::removeGLTF(const std::string& name)
{
	std::erase(gltfNames, name);
	gltfs.erase(name);
}
