#pragma once
#include <filesystem>
#include <functional>
#include <queue>
#include <unordered_map>

#include "Renderer.h"
#include "utils/VKCommon.hpp"
#include "utils/Forwards.h"
#include "geometry/GeoSurface.h"
#include "geometry/Vertex.h"
#include "materials/MetallicRoughness.h"

namespace Imp::Render
{
	enum class MaterialPass : uint8_t;
	class ResourceCache;
}

namespace Imp::Render {
	struct LoadedGLTF;

	//using UniqueCommandBuffer = std::unique_ptr<class CommandBuffer>;

	class VKRenderer final : public Renderer
	{

	private:

		//Core:
		UniqueInstance instance;
		SharedWindow window;
		UniqueDebugMessenger debugMessenger;
		UniqueDevice device;
		UniqueSwapChain swapChain;

		//Queues:
		UniqueQueue graphicsQueue;
		UniqueQueue presentQueue;
		UniqueQueue transferQueue;

		//Commands:
		UniqueCommandPool graphicsPool;
		UniqueCommandPool transferPool;

		//Allocators:
		UniqueVmaAllocator allocator;
		UniqueDescriptorAllocatorGrowable descriptorAllocator;

		//Per Frame:
		std::array<UniqueFrameData, vkutil::MAX_FRAMES_IN_FLIGHT> frames;

		//Images:
		UniqueImage drawImage;
		UniqueImage depthImage;

		//GUI:
		UniqueGUI gui;

		std::unique_ptr<ResourceCache> resourceHandler;
		//Default Data:
		vk::UniqueDescriptorSetLayout gpuSceneDataDescriptorLayout;
		UniqueMetallicRoughness metallicRoughness;
		SharedImage whiteImage;
		SharedImage greyImage;
		SharedImage blackImage;
		SharedImage errorCheckerboardImage;
		vk::UniqueSampler defaultSamplerLinear;
		vk::UniqueSampler defaultSamplerNearest;
		SharedMaterial defaultMaterial;

		std::unique_ptr< DrawContext > drawCtx;

		//Pipelines:
		std::vector<SharedComputePipeline> computePipelines;

		//Renderer Data:
		vk::Extent2D drawExtent{ 0,0 };

		std::chrono::time_point<std::chrono::high_resolution_clock> frameStartTime = std::chrono::high_resolution_clock::now();
		//auto meshDrawStartTime = std::chrono::high_resolution_clock::now();
		//Mesh Data:

		//std::unordered_map<std::string, std::shared_ptr< Node>> loadedNodes;
		//std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedGLTFs;

		std::queue < std::function<void()>> deferredFunctions;



		FrameData& getCurrentFrameData() { return *frames[currentFrame]; }

		void recreate() override;

		SharedMaterial createDefaultMaterial();

	public:
		DISABLE_COPY_AND_MOVE(VKRenderer)
			VKRenderer(const std::string& title, uint32_t width, uint32_t height);
		~VKRenderer() override;


		Vma& getAllocator() const { return *allocator; }
		const Device& getDevice() const { return *device; }
		const SwapChain& getSwapChain() const { return *swapChain; }
		const Image& getDrawImage() const { return *drawImage; }
		const Image& getDepthImage() const { return *depthImage; }
		const SharedImage& getWhiteImage() const { return whiteImage; }
		const SharedImage& getGreyImage() const { return greyImage; }
		const SharedImage& getBlackImage() const { return blackImage; }
		const SharedImage& getErrorCheckerboardImage() const { return errorCheckerboardImage; }
		vk::DescriptorSetLayout getGPUSceneDataDescriptorLayout() const { return *gpuSceneDataDescriptorLayout; }
		auto&& getDefaultSamplerLinear() { return *defaultSamplerLinear; }
		auto&& getDefaultSamplerNearest()  { return *defaultSamplerNearest; }
		auto&& getMetallicRoughness() { return *metallicRoughness; }
		auto&& getTransferQueue() const { return *transferQueue; }
		auto&& getGraphicsQueue() const { return *graphicsQueue; }
		auto&& getTransferPool() const { return *transferPool; }

		ResourceCache& getResourceCache () const { return *resourceHandler; }
		SharedImage createImage(const char* nameId, void* data, const vk::Extent3D size, const vk::Format format, const vk::ImageUsageFlags usage, const
		                        bool mipMapped) const;
		SharedMaterial createMaterial(const char* nameId, MaterialPass type, MetallicRoughness::MaterialResources& resources) const;

		std::shared_ptr<Window> getSharedWindow() override;
		Window& getWindow() override;

		void loadGLTF(std::filesystem::path filePath) override;
		std::shared_ptr<Node> getLoadedGltfNode(const std::string& gltf, const std::string& node) override;

		void loadComputeShader(const std::string& name) override;
		std::weak_ptr<ComputeEffect> getComputeEffect(const std::string& name) override;

		void beginFrame() override;
		void drawCompute() override;

		void drawGeometry() override;

		void startDrawGui() override;
		void endDrawGui() override;
		std::shared_ptr<Imp::Render::Mesh> uploadMesh(const std::string& name, std::span<GeoSurface> surfaces, std::span<uint32_t> indices, std::span<Vertex> vertices) const;
		void endFrame() override;
		ComputeEffect& getCurrentComputeEffect() override;
		int getComputeEffectsSize() override;
		void draw(const ::std::string& gltf, const ::std::string& name, const glm::mat4& transform, const std::string& materialName) override;;
		void draw(const std::string& gltf, const glm::mat4& transform) override;
		void draw(const std::string& name, const std::string& gltf, const std::string& materialName,
				  const glm::mat4& transform) override;
		void windowResized() override;
		void reloadShader(const std::string& name) override;
		std::shared_ptr<Node> getLoadedGltfChildNode(const std::string& gltf, const std::string& child) override;
		const std::vector<std::string>& getLoadedGLTFNames() override;
		const std::vector<std::string>& getLoadedGLTFNodeNames(std::string gltfName) override;
		const std::vector<std::string>& getMaterialNames() override;
	};
}
