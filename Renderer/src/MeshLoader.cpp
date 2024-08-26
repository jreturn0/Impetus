#include "utils/MeshLoader.h"
#include <glm/gtc/quaternion.hpp>
#include "geometry/Mesh.h"
#include <glm/gtx/quaternion.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/core.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include "VKRenderer.h"
#include "core/Device.h"
#include "materials/MetallicRoughness.h"
#include "core/Queue.h"
#include "geometry/Vertex.h"
#include "utils/LoadedGLTF.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Debug.h"
#include "nodes/MeshNode.h"

vk::Filter ExtractFilter(fastgltf::Filter filter)
{
	switch (filter) {
		// nearest samplers
	case fastgltf::Filter::Nearest:
	case fastgltf::Filter::NearestMipMapNearest:
	case fastgltf::Filter::NearestMipMapLinear:
		return vk::Filter::eNearest;

		// linear samplers
	case fastgltf::Filter::Linear:
	case fastgltf::Filter::LinearMipMapNearest:
	case fastgltf::Filter::LinearMipMapLinear:
	default:
		return vk::Filter::eLinear;
	}
}

vk::SamplerMipmapMode ExtractMipmapMode(fastgltf::Filter filter)
{
	switch (filter) {
	case fastgltf::Filter::NearestMipMapNearest:
	case fastgltf::Filter::LinearMipMapNearest:
		return vk::SamplerMipmapMode::eNearest;

	case fastgltf::Filter::NearestMipMapLinear:
	case fastgltf::Filter::LinearMipMapLinear:
	default:
		return vk::SamplerMipmapMode::eLinear;
	}
}
template<typename... Ts>
void print_variant_type(const std::variant<Ts...>& v)
{
	//Debug::Info( "Type index: {}", v.index().);
}

std::optional<Imp::Render::SharedImage> load_image(Imp::Render::VKRenderer& engine, fastgltf::Asset& asset, fastgltf::Image& image)
{
	Imp::Render::SharedImage newImage{};

	int width, height, nrChannels;
	//std::cout << "Loading image: " << image.name << '\n';
	print_variant_type(image.data);

	auto loadImageData = [&](unsigned char* data) -> Imp::Render::SharedImage {
		if (data) {
			const vk::Extent3D imageSize{ static_cast<uint32_t>(width),static_cast<uint32_t>(height),1 };

			newImage = engine.createImage(image.name.c_str(), data, imageSize, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled, true);
			/* newImage = Imp::Render::CreateSharedImage(
				engine.getDevice(),
				engine.getAllocator(),
				engine.getTransferPool(),
				engine.getTransferQueue(),
				data,
				 imageSize,
				vk::Format::eR8G8B8A8Unorm,
				vk::ImageUsageFlagBits::eSampled,
				&engine.getGraphicsQueue().getQueue(), true
			);*/

			stbi_image_free(data);
			return newImage;
		} else {
			Debug::Error("Failed to load image: {}",image.name);
			return nullptr;
		}
		};

	std::visit(
		fastgltf::visitor{
			[](auto& arg) {
				Debug::Error("Unhandled type in variant");
			},

			[&](fastgltf::sources::URI& filePath) {
				//std::cout << "attempting image from uri" << '\n';
				Debug::Assert(filePath.fileByteOffset == 0,"don't support offsets with stbi"); // We don't support offsets with stbi.
				Debug::Assert(filePath.uri.isLocalPath(),"only capable of loading local files"); // We're only capable of loading local files.

				const std::string path(filePath.uri.path().begin(), filePath.uri.path().end()); // Thanks C++.
				unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
				newImage = loadImageData(data);
			},

			[&](fastgltf::sources::Vector& vector) {
				//std::cout << "attempting image from vector" << '\n';
				unsigned char* data = stbi_load_from_memory(
					vector.bytes.data(),
					static_cast<int>(vector.bytes.size()),
					&width,
					&height,
					&nrChannels,
					4
				);
				newImage = loadImageData(data);
			},

			[&](fastgltf::sources::BufferView& view) {
				auto& bufferView = asset.bufferViews[view.bufferViewIndex];
				auto& buffer = asset.buffers[bufferView.bufferIndex];
				print_variant_type(buffer.data);
				std::visit(
					fastgltf::visitor{
						[](auto& arg) {
							Debug::Error( "Unhandled type in buffer view variant");
						},

						[&](fastgltf::sources::Array& vector) {
							//std::cout << "attempting image from buffer view" << '\n';
							unsigned char* data = stbi_load_from_memory(
								vector.bytes.data() + bufferView.byteOffset,
								static_cast<int>(bufferView.byteLength),
								&width,
								&height,
								&nrChannels,
								4
							);
							newImage = loadImageData(data);
						}
					},
					buffer.data
				);
			}
		},
		image.data
	);

	// If any of the attempts to load the data failed, we haven't written the image, so handle is null
	if (newImage == nullptr) {
		Debug::Error("Failed to load image: {}", image.name);
		return {};
	} else {
		return newImage;
	}

}


std::optional<std::shared_ptr<Imp::Render::LoadedGLTF>> Imp::Render::MeshLoader::LoadGltf(std::string_view filePath,
																						  VKRenderer& renderer)
{
	Debug::Info("Loading GLTF: {}", filePath);


	std::shared_ptr<LoadedGLTF> scene = std::make_shared<LoadedGLTF>();
	scene->creator = &renderer;
	LoadedGLTF& file = *scene;

	fastgltf::Parser parser{};

	constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
	// fastgltf::Options::LoadExternalImages;

	fastgltf::GltfDataBuffer data;
	data.loadFromFile(filePath);

	fastgltf::Asset gltf;

	std::filesystem::path path = filePath;

	auto type = fastgltf::determineGltfFileType(&data);
	if (type == fastgltf::GltfType::glTF) {
		auto load = parser.loadGltf(&data, path.parent_path(), gltfOptions);
		if (load) {
			gltf = std::move(load.get());
		} else {
			Debug::Error("Failed to load glTF: {}", fastgltf::to_underlying(load.error()));
			return {};
		}
	} else if (type == fastgltf::GltfType::GLB) {
		auto load = parser.loadGltfBinary(&data, path.parent_path(), gltfOptions);
		if (load) {
			gltf = std::move(load.get());
		} else {
			Debug::Error("Failed to load glTF: {}", fastgltf::to_underlying(load.error()));
			return {};
		}
	} else {
		Debug::Error("Failed to determine glTF container");
		return {};
	}
	// we can stimate the descriptors we will need accurately
	std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = { { vk::DescriptorType::eCombinedImageSampler, 3 },
		{ vk::DescriptorType::eUniformBuffer, 3 },
		{ vk::DescriptorType::eStorageBuffer, 1 } };

	//file.descriptorPool.init_pool(renderer.getDevice().getLogical(), gltf.materials.size(), sizes);
	for (fastgltf::Sampler& sampler : gltf.samplers) {

		vk::SamplerCreateInfo sampl = {};
		sampl.maxLod = vk::LodClampNone;
		sampl.minLod = 0;
		//sampl.addressModeU = vk::SamplerAddressMode::eRepeat;
		//sampl.addressModeV = vk::SamplerAddressMode::eRepeat;
		//sampl.addressModeW = vk::SamplerAddressMode::eRepeat;
		sampl.magFilter = ExtractFilter(sampler.magFilter.value_or(fastgltf::Filter::Nearest));
		sampl.minFilter = ExtractFilter(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

		sampl.mipmapMode = ExtractMipmapMode(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

		file.samplers.push_back(renderer.getDevice().getLogical().createSamplerUnique(sampl));
	}
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Node>> nodes;
	std::vector<SharedImage> images;
	std::vector<std::shared_ptr<Material>> materials;

	for (fastgltf::Image& image : gltf.images) {
		auto img = load_image(renderer, gltf, image);
		//std::cout << "Loaded image: " << image.name << '\n';
		if (img.has_value()) {
			images.push_back(*img);
			file.images[image.name.c_str()] = *img;
		} else {
			// we failed to load, so lets give the slot a default white texture to not
			// completely break loading
			images.push_back(renderer.getErrorCheckerboardImage());
			Debug::FatalError( "Failed to load texture: {}", image.name);
		}
	}


	file.materialDataBuffer = CreateSharedBuffer(renderer.getAllocator(), sizeof(MetallicRoughness::MaterialConstants) * gltf.materials.size(),
												 vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
	int data_index = 0;
	int transparentMaterials = 0;
	MetallicRoughness::MaterialConstants* sceneMaterialConstants = static_cast<MetallicRoughness::MaterialConstants*>(file.materialDataBuffer->getAllocationInfo().pMappedData);
	for (fastgltf::Material& mat : gltf.materials) {


		MetallicRoughness::MaterialConstants constants;
		constants.colorFactors.x = mat.pbrData.baseColorFactor[0];
		constants.colorFactors.y = mat.pbrData.baseColorFactor[1];
		constants.colorFactors.z = mat.pbrData.baseColorFactor[2];
		constants.colorFactors.w = mat.pbrData.baseColorFactor[3];

		constants.metalRoughFactors.x = mat.pbrData.metallicFactor;
		constants.metalRoughFactors.y = mat.pbrData.roughnessFactor;
		// write material parameters to buffer
		sceneMaterialConstants[data_index] = constants;

		MaterialPass passType = MaterialPass::MainColor;
		if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
			passType = MaterialPass::Transparent;
			++transparentMaterials;
		}

		MetallicRoughness::MaterialResources materialResources;
		// default the material textures
		materialResources.colorImage = renderer.getWhiteImage();
		materialResources.colorSampler = &renderer.getDefaultSamplerNearest();
		materialResources.metalRoughImage = renderer.getWhiteImage();
		materialResources.metalRoughSampler = &renderer.getDefaultSamplerLinear();

		// set the uniform buffer for the material data
		materialResources.dataBuffer = file.materialDataBuffer;
		materialResources.dataOffset = data_index * sizeof(MetallicRoughness::MaterialConstants);
		// grab textures from gltf file
		if (mat.pbrData.baseColorTexture.has_value()) {
			size_t img = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
			size_t sampler = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

			materialResources.colorImage = images[img];
			materialResources.colorSampler = &*(file.samplers[sampler]);
		}
		if (mat.pbrData.metallicRoughnessTexture.has_value()) {
			size_t img = gltf.textures[mat.pbrData.metallicRoughnessTexture.value().textureIndex].imageIndex.value();
			size_t sampler = gltf.textures[mat.pbrData.metallicRoughnessTexture.value().textureIndex].samplerIndex.value();

			materialResources.metalRoughImage = images[img];
			materialResources.metalRoughSampler = &*(file.samplers[sampler]);
		}

		// build material
		auto newMat = renderer.createMaterial(mat.name.c_str(), passType, materialResources);
		materials.push_back(newMat);
		file.materials[mat.name.c_str()] = newMat;
		data_index++;
	}
	//std::cout << "Loaded " << gltf.materials.size() << " materials, " << transparentMaterials << " are transparent" <<'\n';
	// use the same vectors for all meshes so that the memory doesnt reallocate as
// often
	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;

	for (fastgltf::Mesh& mesh : gltf.meshes) {
		std::shared_ptr<Mesh> newmesh = std::make_shared<Mesh>();
		meshes.push_back(newmesh);
		file.meshes[mesh.name.c_str()] = newmesh;
		newmesh->name = mesh.name;

		// clear the mesh arrays each mesh, we dont want to merge them by error
		indices.clear();
		vertices.clear();

		for (auto&& p : mesh.primitives) {
			GeoSurface newSurface;
			newSurface.startIndex = static_cast<uint32_t>(indices.size());
			newSurface.count = static_cast<uint32_t>(gltf.accessors[p.indicesAccessor.value()].count);

			size_t initial_vtx = vertices.size();

			// load indexes
			{
				fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
				indices.reserve(indices.size() + indexaccessor.count);

				fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor,
														 [&](std::uint32_t idx) {
															 indices.push_back(idx + initial_vtx);
														 });
			}

			// load vertex positions
			{
				fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->second];
				vertices.resize(vertices.size() + posAccessor.count);

				fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
															  [&](glm::vec3 v, size_t index) {
																  Vertex newvtx;
																  newvtx.position = v;
																  newvtx.normal = { 1, 0, 0 };
																  newvtx.color = glm::vec4{ 1.f };
																  newvtx.uv_x = 0;
																  newvtx.uv_y = 0;
																  vertices[initial_vtx + index] = newvtx;
															  });
			}

			// load vertex normals
			auto normals = p.findAttribute("NORMAL");
			if (normals != p.attributes.end()) {

				fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).second],
															  [&](glm::vec3 v, size_t index) {
																  vertices[initial_vtx + index].normal = v;
															  });
			}

			// load UVs
			auto uv = p.findAttribute("TEXCOORD_0");
			if (uv != p.attributes.end()) {

				fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).second],
															  [&](glm::vec2 v, size_t index) {
																  vertices[initial_vtx + index].uv_x = v.x;
																  vertices[initial_vtx + index].uv_y = v.y;
															  });
			}

			// load vertex colors
			auto colors = p.findAttribute("COLOR_0");
			if (colors != p.attributes.end()) {

				fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).second],
															  [&](glm::vec4 v, size_t index) {
																  vertices[initial_vtx + index].color = v;
															  });
			}

			if (p.materialIndex.has_value()) {
				//newSurface.materialName = gltf.materials[p.materialIndex.value()].name;
				newSurface.material = materials[p.materialIndex.value()];
			} else {
				//newSurface.materialName = gltf.materials[0].name;
				newSurface.material = materials[0];
			}

			glm::vec3 minpos = vertices[initial_vtx].position;
			glm::vec3 maxpos = vertices[initial_vtx].position;
			for (int i = initial_vtx; i < vertices.size(); i++) {
				minpos = glm::min(minpos, vertices[i].position);
				maxpos = glm::max(maxpos, vertices[i].position);
			}
			// calculate origin and extents from the min/max, use extent lenght for radius
			newSurface.bounds.origin = (maxpos + minpos) / 2.f;
			newSurface.bounds.extents = (maxpos - minpos) / 2.f;
			newSurface.bounds.sphereRadius = glm::length(newSurface.bounds.extents);

			newmesh->surfaces.push_back(newSurface);
		}
		//std::cout << "Mesh: " << newmesh->name << " has " << newmesh->surfaces.size() << " surfaces" << '\n';
		newmesh->meshBuffer = std::make_unique< GPUMeshBuffers>(renderer.getDevice(), renderer.getTransferQueue().getQueue(),
																renderer.getTransferPool(), renderer.getAllocator(), indices, vertices);
	}
	for (fastgltf::Node& node : gltf.nodes) {
		std::shared_ptr<Node> newNode;
		// find if the node has a mesh, and if it does hook it to the mesh pointer and allocate it with the meshnode class
		if (node.meshIndex.has_value()) {
			newNode = std::make_shared<MeshNode>();
			dynamic_cast<MeshNode*>(newNode.get())->mesh = meshes[*node.meshIndex];
			//std::cout << "Node: " << node.name << " has mesh: " << meshes[*node.meshIndex]->name << '\n';
		} else {
			newNode = std::make_shared<Node>();
		}

		file.nodeNames.push_back(node.name.c_str());
		nodes.push_back(newNode);
		file.nodes[node.name.c_str()] = newNode;

		std::visit(fastgltf::visitor{ [&](fastgltf::Node::TransformMatrix matrix) {
										  memcpy(&newNode->localTransform, matrix.data(), sizeof(matrix));
									  },
					   [&](const fastgltf::TRS& transform) {

						   glm::vec3 tl(transform.translation[0], transform.translation[1],
							   transform.translation[2]);
						   glm::quat rot(transform.rotation[3], transform.rotation[0], transform.rotation[1],
							   transform.rotation[2]);
						   glm::vec3 sc(transform.scale[0], transform.scale[1], transform.scale[2]);

						   glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
						   glm::mat4 rm = glm::toMat4(rot);
						   glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);

						   newNode->localTransform = tm * rm * sm;
					   } },
				   node.transform);

	}


	// run loop again to setup transform hierarchy
	for (int i = 0; i < gltf.nodes.size(); i++) {
		fastgltf::Node& node = gltf.nodes[i];
		std::shared_ptr<Node>& sceneNode = nodes[i];
		for (auto& c : node.children) {
			sceneNode->children.push_back(nodes[c]);
			nodes[c]->parent = sceneNode;
		}
	}

	// find the top nodes, with no parents
	for (auto& node : nodes) {
		if (node->parent.lock() == nullptr) {
			file.topNodes.push_back(node);
			node->refreshTransform(glm::mat4{ 1.f });
		}
	}
	file.nodeNames.emplace_back("Root");
	return scene;





}
