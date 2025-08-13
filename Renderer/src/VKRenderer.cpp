#include "VKRenderer.h"




#include "core/CommandPool.h"
#include "core/ComputeEffect.h"
#include "core/Device.h"
#include "core/Image.h"
#include "core/SwapChain.h"
#include "core/ComputePipeline.h"
#include "core/DebugMessenger.h"
#include "utils/descriptor/DescriptorLayoutBuilder.h"
#include "core/FrameData.h"
#include "gpudata/GPUDrawPushConstants.h"
#include "core/GraphicsPipeline.h"
#include "geometry//Mesh.h"
#include "materials/MetallicRoughness.h"
#include "nodes/Node.h"
#include "core/Queue.h"
#include "window/VKWindow.h"
#include "utils/GUI.h"
#include "utils/MeshLoader.h"
#include "core/ImmediateCommands.h"
#include "core/Instance.h"
#include "core/Vma.h"
#include <glm/gtc/type_ptr.hpp>

#include <glm/simd/matrix.h>

#include "rendercontext/DrawContext.h"
#include "rendercontext/RenderObject.h"
#include "utils/LoadedGLTF.h"
#include <immintrin.h>
#include <mutex>
#include <execution>
#include <algorithm>
#include <glm/packing.hpp>

#include "Debug.h"
#include "Log.h"
#include "utils/ResourceCache.h"



namespace Imp::Render {
	// Anonymous namespace for utility functions only used here
	namespace {
		void SetViewportAndScissors(const vk::CommandBuffer& cmd, const vk::Extent2D extent)
		{
			vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f);
			cmd.setViewport(0, viewport);

			vk::Rect2D scissor({ 0, 0 }, extent);
			cmd.setScissor(0, scissor);
		}

		auto GetColorAttachment(const vk::ImageView& view, vk::AttachmentLoadOp op, vk::ClearColorValue clear = { 0.f,0.f,0.f,1.f })
		{
			return vk::RenderingAttachmentInfoKHR{
				view,
				vk::ImageLayout::eColorAttachmentOptimal,
				{},{},{},
				op,
				vk::AttachmentStoreOp::eStore,
				clear
			};
		}

		auto GetDepthAttachment(const vk::ImageView* view, vk::AttachmentLoadOp op, vk::ClearDepthStencilValue clear = { 1.f,0 })
		{

			return vk::RenderingAttachmentInfoKHR{
				 view != nullptr ? *view : vk::ImageView{},
				vk::ImageLayout::eDepthAttachmentOptimal,
				{},{},{},
				op,
				vk::AttachmentStoreOp::eStore,
				clear
			};
		}

		void SetBeginRendering(const vk::CommandBuffer& cmd, const vk::Extent2D extent, const vk::ImageView& colour, vk::AttachmentLoadOp loadOp, const vk::ImageView* depth = nullptr, vk::ClearColorValue clear = { 0.f,0.f,0.f,1.f })
		{
			auto colourAttachment = GetColorAttachment(colour, loadOp, clear);

			auto depthAttachment = depth != nullptr ? GetDepthAttachment(depth, vk::AttachmentLoadOp::eClear, { 1.f,0 }) : vk::RenderingAttachmentInfoKHR{};

			vk::RenderingInfoKHR renderingInfo(
				{}, // flags
				vk::Rect2D(vk::Offset2D(0, 0), extent), // renderArea
				1, // layerCount
				0, // viewMask
				1, // colorAttachmentCount
				&colourAttachment, // pColorAttachments
				depth != nullptr ? &depthAttachment : nullptr, // pDepthAttachment
				nullptr // pStencilAttachment
			);

			cmd.beginRendering(renderingInfo);
		}

		vk::Format ChooseSupportedFormat(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
		{
			for (vk::Format format : candidates) {
				vk::FormatProperties props = physicalDevice.getFormatProperties(format);

				if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
					return format;
				}
				else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
					return format;
				}
			}
			throw std::runtime_error("Failed to find supported format!");
		}

		auto CreateWhiteImage(const VKRenderer& renderer)
		{
			uint32_t color = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));

			return renderer.createImage("white_image", (void*)&color, { 1,1,1 },
				vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled, true);

		}
		auto CreateGreyImage(const VKRenderer& renderer)
		{
			uint32_t color = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
			return renderer.createImage("grey_image", (void*)&color, { 1,1,1 },
				vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled, true);
		}
		auto CreateBlackImage(const VKRenderer& renderer)
		{
			uint32_t color = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));
			return renderer.createImage("black_image", (void*)&color, { 1,1,1 },
				vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled, true);
		}
		auto CreateErrorImage(const VKRenderer& renderer)
		{
			uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
			uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
			std::array<uint32_t, 16 * 16 > pixels{}; //for 16x16 checkerboard texture
			for (int x = 0; x < 16; x++) {
				for (int y = 0; y < 16; y++) {
					pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
				}
			}
			return renderer.createImage("error_image", (void*)&pixels[0], { 16,16,1 },
				vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled, true);
		}

		auto CreateGPUSceneDescriptorLayout(const Imp::Render::Device& device)
		{
			Imp::Render::DescriptorLayoutBuilder builder;

			builder.addBinding(0, vk::DescriptorType::eUniformBuffer);

			return builder.build(device.getLogical(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

		}

		bool isBoundingBoxVisible(const RenderObject& obj, const glm::mat4& viewProj)
		{
			static const std::array<glm::vec4, 8> corners = {
			glm::vec4{1, 1, 1, 1},
			glm::vec4{1, 1, -1, 1},
			glm::vec4{1, -1, 1, 1},
			glm::vec4{1, -1, -1, 1},
			glm::vec4{-1, 1, 1, 1},
			glm::vec4{-1, 1, -1, 1},
			glm::vec4{-1, -1, 1, 1},
			glm::vec4{-1, -1, -1, 1}
			};

			glm::mat4 matrix = viewProj * obj.transform;
			glm::vec3 min_(1.5f);
			glm::vec3 max_(-1.5f);

			for (const auto& corner : corners) {
				glm::vec4 transformedCorner = glm::vec4(obj.bounds.origin + obj.bounds.extents * glm::vec3(corner), 1.0f);
				glm::vec4 v = matrix * transformedCorner;
				v /= v.w;
				min_ = glm::min(min_, glm::vec3(v));
				max_ = glm::max(max_, glm::vec3(v));
			}

			return !(min_.z > 1.0f || max_.z < 0.0f || min_.x > 1.0f || max_.x < -1.0f || min_.y > 1.0f || max_.y < -1.0f);
		}
	}
}
void Imp::Render::VKRenderer::recreate()
{
	device->getLogical().waitIdle();
	swapChain->RecreateSwapChain(*device, *window);
	device->getLogical().waitIdle();
	vk::Format drawImageFormat = ChooseSupportedFormat(
		device->getPhysical(),
		{ vk::Format::eB8G8R8A8Srgb, vk::Format::eR16G16B16A16Sfloat },
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eStorageImage | vk::FormatFeatureFlagBits::eColorAttachment
	);
	drawImage = CreateDrawImage(*device, *allocator, swapChain->getExtent(), drawImageFormat);
	depthImage = CreateDepthImage(*device, *allocator, swapChain->getExtent());
	for (auto& compute : computePipelines) {
		compute->recreate(*device, *drawImage, *descriptorAllocator);
	}
	device->getLogical().waitIdle();
}

Imp::Render::SharedMaterial Imp::Render::VKRenderer::createDefaultMaterial()
{
	MetallicRoughness::MaterialResources materialResources;
	//default the material textures
	materialResources.colorImage = errorCheckerboardImage;
	materialResources.colorSampler = &defaultSamplerLinear.get();
	materialResources.metalRoughImage = errorCheckerboardImage;
	materialResources.metalRoughSampler = &defaultSamplerLinear.get();

	auto materialConstants = CreateSharedBuffer(*allocator, sizeof(MetallicRoughness::MaterialConstants), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);;
	//write the buffer
	MetallicRoughness::MaterialConstants* sceneUniformData = static_cast<MetallicRoughness::MaterialConstants*>(materialConstants->getAllocationInfo().pMappedData);
	sceneUniformData->colorFactors = glm::vec4{ 1,1,1,1 };
	sceneUniformData->metalRoughFactors = glm::vec4{ 1,0.5,0,0 };



	materialResources.dataBuffer = std::move(materialConstants);
	materialResources.dataOffset = 0;
	materialConstants = nullptr;

	return metallicRoughness->writeMaterial(*device, MaterialPass::MainColor, materialResources, *descriptorAllocator);
}

Imp::Render::VKRenderer::VKRenderer(const std::string& title, uint32_t width, uint32_t height) :
	instance(CreateUniqueInstance(title.c_str())),
	window(std::make_shared<VKWindow>(width, height, title.c_str(), *instance)),
	debugMessenger(CreateUniqueDebugMessenger(*instance)),
	device(CreateUniqueDevice(*instance, *window)),
	swapChain(CreateUniqueSwapChain(*device, *window)),
	graphicsQueue(CreateUniqueQueue(*device, QueueFamily::Graphics)),
	presentQueue(CreateUniqueQueue(*device, QueueFamily::Present)),
	transferQueue(CreateUniqueQueue(*device, QueueFamily::Transfer)),
	allocator(std::make_unique<Vma>(*instance, *device)),
	descriptorAllocator(CreateUniqueDescriptorAllocatorGrowableEasyBake(*device)),
	graphicsPool(CreateUniqueCommandPool(*device, device->getIndices().graphicsFamily.value())),
	transferCommands(std::make_unique<ImmediateCommands>(*device, device->getIndices().transferFamily.value())),
	frames(CreateFrameDataArray(*device, *swapChain, *graphicsPool)),
	drawImage(CreateDrawImage(*device, *allocator, swapChain->getExtent(), swapChain->getFormat())),
	depthImage(CreateDepthImage(*device, *allocator, swapChain->getExtent())),
	gui(std::make_unique<GUI>(*instance, *device, *swapChain, *graphicsQueue, *window)),
	//default data and pipelines
	gpuSceneDataDescriptorLayout(CreateGPUSceneDescriptorLayout(*device)),
	metallicRoughness(std::make_unique<MetallicRoughness>(*this)),
	whiteImage(CreateWhiteImage(*this)),
	greyImage(CreateGreyImage(*this)),
	blackImage(CreateBlackImage(*this)),
	errorCheckerboardImage(CreateErrorImage(*this)),
	defaultSamplerLinear(device->getLogical().createSamplerUnique(vk::SamplerCreateInfo{ {},vk::Filter::eLinear,vk::Filter::eLinear, })),
	defaultSamplerNearest(device->getLogical().createSamplerUnique(vk::SamplerCreateInfo{ {},vk::Filter::eNearest,vk::Filter::eNearest })),
	defaultMaterial(createDefaultMaterial()),
	resourceHandler(std::make_unique<ResourceCache>(*device)),
	drawCtx(std::make_unique<DrawContext>())
{
	;
	MetallicRoughness::MaterialResources materialResources = MetallicRoughness::MaterialResources{
	errorCheckerboardImage,
	&*defaultSamplerNearest,
	errorCheckerboardImage,
	&*defaultSamplerNearest,
	CreateSharedBuffer(*allocator,
					   sizeof(MetallicRoughness::MaterialConstants),
					   vk::BufferUsageFlagBits::eUniformBuffer,
					   VMA_MEMORY_USAGE_CPU_TO_GPU),0
	};

	auto errorMaterial = metallicRoughness->writeMaterial(*device, MaterialPass::MainColor, materialResources
		, resourceHandler->getMaterialDescriptorAllocator());
	resourceHandler->addMaterial("error_material", errorMaterial);

}






Imp::Render::VKRenderer::~VKRenderer()
{
	device->getLogical().waitIdle();
}

Imp::Render::SharedImage Imp::Render::VKRenderer::createImage(const char* nameId, void* data, const vk::Extent3D size, const vk::Format format, const vk::ImageUsageFlags usage, const bool mipMapped) const
{

	auto image = CreateSharedImage(*device, *allocator, *transferCommands, *transferQueue, data, size, format, usage, &graphicsQueue->getQueue(), mipMapped);
	if (image == nullptr) {
		//throw std::runtime_error("Failed to create image");
	}
	if (resourceHandler->hasImage(nameId)) {
		//std::cout << "Image with name (" << nameId << ") already exists" << std::endl;
		//	throw std::runtime_error("Image with name already exists");
	}
	resourceHandler->addImage(nameId, image);
	return image;
}

Imp::Render::SharedMaterial Imp::Render::VKRenderer::createMaterial(const char* nameId, MaterialPass type, MetallicRoughness::MaterialResources& resources) const
{

	auto material = metallicRoughness->writeMaterial(*device, type, resources, resourceHandler->getMaterialDescriptorAllocator());
	material->name = nameId;
	if (material == nullptr) {
		//throw std::runtime_error("Failed to create material");
	}

	if (resourceHandler->hasMaterial(nameId)) {
		//std::cout << "Material with name (" << nameId << ") already exists" << std::endl;
		//throw std::runtime_error("Material with name already exists");
	}

	resourceHandler->addMaterial(nameId, material);
	return material;
}

std::shared_ptr<Imp::Window> Imp::Render::VKRenderer::getSharedWindow()
{
	return window;
}

Imp::Window& Imp::Render::VKRenderer::getWindow()
{
	return *window;
}

void Imp::Render::VKRenderer::loadGLTF(std::filesystem::path filePath)
{

	if (resourceHandler->hasGLTF(filePath.string())) {
		return;
	}

	auto gltf = MeshLoader::LoadGltf(filePath.string(), *this);
	if (!gltf.has_value()) {
		Debug::FatalError("Failed to load gltf file");
		//std::cout << "Failed to load gltf file" << std::endl;
		//throw std::runtime_error("Failed to load gltf file");
		return;
	}
	resourceHandler->addGLTF(filePath.filename().string(), gltf.value());
	//loadedGLTFs[filePath.string()] = *gltf;

}

std::shared_ptr<Imp::Render::Node> Imp::Render::VKRenderer::getLoadedGltfNode(const std::string& gltf, const std::string& node)
{
	return resourceHandler->getGLTF(gltf)->getNode(node);
}

void Imp::Render::VKRenderer::loadComputeShader(const std::string& name)
{
	computePipelines.push_back(CreateUniqueComputePipeline(*device, *drawImage, *descriptorAllocator, name));

}

std::weak_ptr<Imp::ComputeEffect> Imp::Render::VKRenderer::getComputeEffect(const std::string& name)
{
	auto result = std::ranges::find_if(computePipelines, [name](const auto& pipeline) {return pipeline->getName() == name; });
	if (result == computePipelines.end()) {
		return {};
	}
	return (*result);
}


void Imp::Render::VKRenderer::beginFrame()
{
	frameStartTime = std::chrono::high_resolution_clock::now();
	auto start = std::chrono::high_resolution_clock::now();
	auto&& frame = getCurrentFrameData();
	const vk::CommandBuffer& cmd = frame.getCommandBuffer();
	auto& logical = device->getLogical();
	drawExtent.height = std::min(swapChain->getExtent().height, drawImage->getExtent().height) * renderScale;
	drawExtent.width = std::min(swapChain->getExtent().width, drawImage->getExtent().width) * renderScale;

	vkutil::CheckResult(logical.waitForFences(frame.getInFlightFence(), VK_TRUE, UINT64_MAX));
	vk::Result result = logical.acquireNextImageKHR(*swapChain, UINT64_MAX, frame.getImageAvailableSemaphore(), {}, &imageIndex);
	if (result == vk::Result::eErrorOutOfDateKHR) {
		recreate();
		isWindowResized = false;
		return;
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}
	logical.resetFences(frame.getInFlightFence());
	frame.getCommandBuffer().reset();

	vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
	if (cmd.begin(&beginInfo) != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}
	auto end = std::chrono::high_resolution_clock::now();
	stats.timeStatsMap["beginFrame"].update(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

}

void Imp::Render::VKRenderer::drawCompute()
{
	drawImage->transitionImageLayout(getCurrentFrameData().getCommandBuffer(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
	if (computePipelines.empty()) {
		return;
	}
	auto& cmd = getCurrentFrameData().getCommandBuffer();
	//auto start = std::chrono::high_resolution_clock::now();
	computePipelines[currentComputePipeline]->bind(cmd);
	computePipelines[currentComputePipeline]->dispatch(cmd, std::ceil(drawImage->getExtent().width / 16.0), std::ceil(drawImage->getExtent().height / 16.0), 1);
	//auto end = std::chrono::high_resolution_clock::now();
	//stats.computeDrawTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()/1000.f;
}



void Imp::Render::VKRenderer::drawGeometry()
{
	auto start = std::chrono::high_resolution_clock::now();

	auto& cmd = getCurrentFrameData().getCommandBuffer();

	auto start2 = std::chrono::high_resolution_clock::now();

	drawImage->transitionImageLayout(cmd, vk::ImageLayout::eGeneral, vk::ImageLayout::eColorAttachmentOptimal);
	depthImage->transitionImageLayout(cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal);

	stats.drawCallCount = 0;
	stats.triangleCount = 0;


	SetBeginRendering(cmd, drawExtent, drawImage->getView(), vk::AttachmentLoadOp::eDontCare, &depthImage->getView());
	auto end2 = std::chrono::high_resolution_clock::now();
	stats.timeStatsMap["transitionAndBeginRendering"].update(std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count());
	auto start3 = start2 = std::chrono::high_resolution_clock::now();
	std::vector<uint32_t> opaque_draws;
	opaque_draws.reserve(drawCtx->opaque.size());

	std::mutex drawMutex;

	std::for_each(std::execution::par, drawCtx->opaque.begin(), drawCtx->opaque.end(), [&](const RenderObject& obj) {
		if (!culling) {
			std::lock_guard<std::mutex> lock(drawMutex);
			opaque_draws.push_back(&obj - drawCtx->opaque.data());
			return;
		}

		if (isBoundingBoxVisible(obj, sceneData.viewproj)) {
			std::lock_guard<std::mutex> lock(drawMutex);
			opaque_draws.push_back(&obj - drawCtx->opaque.data());
		}
		});



	end2 = std::chrono::high_resolution_clock::now();
	stats.timeStatsMap["visibilityCheck"].update(std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count());
	start3 = std::chrono::high_resolution_clock::now();
	std::ranges::sort(opaque_draws, [&](const auto& iA, const auto& iB) {
		const RenderObject& A = drawCtx->opaque[iA];
		const RenderObject& B = drawCtx->opaque[iB];
		if (A.material == B.material) {
			return A.indexBuffer < B.indexBuffer;
		}
		else {
			return A.material < B.material;
		}
		});
	end2 = std::chrono::high_resolution_clock::now();
	stats.timeStatsMap["sort"].update(std::chrono::duration_cast<std::chrono::microseconds>(end2 - start3).count());
	stats.timeStatsMap["visibilityCheckAndSort"].update(std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count());
	start2 = std::chrono::high_resolution_clock::now();
	auto& gpuSceneDataBuffer = getCurrentFrameData().getSceneDataBuffer_();
	gpuSceneDataBuffer = CreateUniqueBuffer(*allocator, sizeof(GPUSceneData), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);

	//getCurrentFrameData().deferDelete([=, this]() {
	//	gpuSceneDataBuffer->destroy();
	//								  });

	GPUSceneData* sceneUniformData = static_cast<GPUSceneData*>(gpuSceneDataBuffer->getAllocationInfo().pMappedData);
	*sceneUniformData = sceneData;

	auto& globalDescriptor = getCurrentFrameData().getSceneDataDescriptorSet_();
	globalDescriptor = getCurrentFrameData().getDescriptorAllocator().allocate(*device, *gpuSceneDataDescriptorLayout);

	DescriptorWriter writer;
	writer.write_buffer(0, gpuSceneDataBuffer->getBuffer(), sizeof(GPUSceneData), 0, vk::DescriptorType::eUniformBuffer);
	writer.update_set(*device, *globalDescriptor);

	end2 = std::chrono::high_resolution_clock::now();
	stats.timeStatsMap["updateGlobalDescriptor"].update(std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count());
	start2 = std::chrono::high_resolution_clock::now();
	GraphicsPipeline* lastPipeline = nullptr;
	std::shared_ptr<Material> lastMaterial = nullptr;
	vk::Buffer lastIndexBuffer = VK_NULL_HANDLE;

	auto draw = [&](const RenderObject& robj) {
		if (robj.material != lastMaterial) {
			lastMaterial = robj.material;

			if (robj.material->pipeline != lastPipeline) {
				//	std::cout << "swapping pipelines\n";
				lastPipeline = robj.material->pipeline;
				robj.material->pipeline->bind(cmd);
				cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, robj.material->pipeline->getLayout(), 0, *globalDescriptor, {});
				SetViewportAndScissors(cmd, drawExtent);
			}
			cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, robj.material->pipeline->getLayout(), 1, *robj.material->set, {});
		}
		if (robj.indexBuffer != lastIndexBuffer) {
			lastIndexBuffer = robj.indexBuffer;
			cmd.bindIndexBuffer(robj.indexBuffer, 0, vk::IndexType::eUint32);
		}
		GPUDrawPushConstants pushConstants{ robj.transform, robj.vertexBufferAddress };
		cmd.pushConstants<GPUDrawPushConstants>(robj.material->pipeline->getLayout(), vk::ShaderStageFlagBits::eVertex, 0, pushConstants);
		cmd.drawIndexed(robj.indexCount, 1, robj.firstIndex, 0, 0);
		++stats.drawCallCount;
		stats.triangleCount += robj.indexCount / 3;
		};

	for (auto& r : opaque_draws) {
		draw(drawCtx->opaque[r]);
	}
	drawCtx->opaque.clear();

	for (auto&& mesh : drawCtx->transparent) {
		draw(mesh);
	}
	drawCtx->transparent.clear();

	cmd.endRendering();
	end2 = std::chrono::high_resolution_clock::now();
	stats.timeStatsMap["drawingGeo"].update(std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count());
	auto end = std::chrono::high_resolution_clock::now();
	stats.timeStatsMap["drawGeometry"].update(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
}



void Imp::Render::VKRenderer::startDrawGui()
{
	gui->startFrame();
}

void Imp::Render::VKRenderer::endDrawGui()
{
	auto start = std::chrono::high_resolution_clock::now();
	gui->endFrame();
	//auto start = std::chrono::high_resolution_clock::now();
	const vk::CommandBuffer& cmd = frames[currentFrame]->getCommandBuffer();
	SetBeginRendering(cmd, drawImage->getExtent2D(), drawImage->getView(), vk::AttachmentLoadOp::eDontCare);
	SetViewportAndScissors(cmd, drawImage->getExtent2D());
	gui->render(cmd);
	cmd.endRendering();
	auto end = std::chrono::high_resolution_clock::now();
	stats.timeStatsMap["drawGui"].update(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
	//auto end = std::chrono::high_resolution_clock::now();
	//stats.guiDrawTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.f;

}
std::shared_ptr<Imp::Render::Mesh> Imp::Render::VKRenderer::uploadMesh(const std::string& name,
	std::span<GeoSurface> surfaces,
	std::span<uint32_t> indices,
	std::span<Vertex> vertices) const
{
	return std::make_shared<Mesh>(name, *device, *transferQueue, *transferCommands, *allocator, surfaces, indices, vertices);
}
void Imp::Render::VKRenderer::endFrame()
{
	auto start = std::chrono::high_resolution_clock::now();
	const vk::CommandBuffer& cmd = frames[currentFrame]->getCommandBuffer();
	auto& frame = frames[currentFrame];


	drawImage->transitionImageLayout(cmd, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal);
	TransitionImageLayout(swapChain->getImages()[imageIndex], cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	CopyImageToSwapChain(cmd, *drawImage, *swapChain, imageIndex);
	TransitionImageLayout(swapChain->getImages()[imageIndex], cmd, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR);



	cmd.end();
	// Submit the command buffer for execution
	vk::PipelineStageFlags2 waitStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
	vk::SemaphoreSubmitInfo waitSemaphoreInfo(frame->getImageAvailableSemaphore(), 0, waitStage);
	vk::CommandBufferSubmitInfo commandBufferInfo(frame->getCommandBuffer());
	vk::SemaphoreSubmitInfo signalSemaphoreInfo(frame->getRenderFinishedSemaphore(), 0, vk::PipelineStageFlagBits2::eBottomOfPipe);
	vk::SubmitInfo2 submitInfo2({}, waitSemaphoreInfo, commandBufferInfo, signalSemaphoreInfo);

	if (graphicsQueue->getQueue().submit2(1, &submitInfo2, frame->getInFlightFence()) != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	vk::PresentInfoKHR presentInfo(
		1, &frame->getRenderFinishedSemaphore(),
		1, &swapChain->getSwapChain(),
		&imageIndex
	);

	vk::Result result = presentQueue->getQueue().presentKHR(&presentInfo);
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
		recreate();
		isWindowResized = false;
	}
	else if (result != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to present swap chain image!");
	}

	++frameNumber;
	currentFrame = (currentFrame + 1) % vkutil::MAX_FRAMES_IN_FLIGHT;
	device->getLogical().waitIdle();
	getCurrentFrameData().flushDeletions();

	while (!deferredFunctions.empty()) {
		//std::cout << "Running deferred function" << std::endl;
		//std::cout << "Deferred functions size: " << deferredFunctions.size() << std::endl;
		deferredFunctions.front()();
		deferredFunctions.pop();
	}
	auto frameEndTime = std::chrono::high_resolution_clock::now();
	stats.timeStatsMap["endFrame"].update(std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - start).count());
	stats.timeStatsMap["totalFrame"].update(std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime).count());

}

Imp::ComputeEffect& Imp::Render::VKRenderer::getCurrentComputeEffect()
{
	return *computePipelines[currentComputePipeline];
}

int Imp::Render::VKRenderer::getComputeEffectsSize()
{
	return computePipelines.size();
}
void Imp::Render::VKRenderer::draw(const std::string& gltf, const std::string& name, const glm::mat4& transform, const std::string& materialName = "")
{
	// Check if the GLTF exists in the resource handler
	if (!resourceHandler->hasGLTF(gltf)) {
		//std::cout << "GLTF with name (" << gltf << ") does not exist" << std::endl;
		return;
	}

	// Retrieve the GLTF node
	const auto&& gltfNode = resourceHandler->getGLTF(gltf);
	if (!gltfNode) {
		//std::cout << "GLTF with name (" << gltf << ") does not exist" << std::endl;
		return;
	}
	if (name.empty() || name == "Root") {
		// Draw the entire GLTF
		gltfNode->draw(transform, *drawCtx);
		return;
	}

	if (auto&& node = gltfNode->getNode(name); node != nullptr) {
		// Draw the node with or without a specific material
		if (materialName.empty() || materialName == "default") {
			node->draw(transform, *drawCtx);
		}
		else {
			if (!resourceHandler->hasMaterial(materialName)) {
				//std::cout << "Material with name (" << materialName << ") does not exist" << std::endl;
				node->draw(transform, *drawCtx, resourceHandler->getMaterialRef("error_material"));
			}
			else {
				node->draw(transform, *drawCtx, resourceHandler->getMaterialRef(materialName));
			}
			//node->draw(transform, *drawCtx, resourceHandler->getMaterialRef(materialName));
		}
	}
	else {
		//std::cout << "GLTF(" << gltf << ") node with name (" << name << ") does not exist" << std::endl;
	}


}

void Imp::Render::VKRenderer::draw(const std::string& gltf, const glm::mat4& transform)
{
	draw(gltf, "Root", transform, "");
}

void Imp::Render::VKRenderer::draw(const std::string& name, const std::string& gltf, const std::string& materialName, const glm::mat4& transform)
{
	draw(gltf, name, transform, materialName);
}


void Imp::Render::VKRenderer::windowResized()
{
	Renderer::windowResized();
	int width = 0, height = 0;
	glfwGetFramebufferSize(window->getGLFW(), &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window->getGLFW(), &width, &height);
		glfwWaitEvents();
	}
	window->width = width;
	window->height = height;
}

void Imp::Render::VKRenderer::reloadShader(const std::string& name)
{
	deferredFunctions.emplace([this, name]() {
		ShaderCompiler::Instance().CheckCompileAll();
		auto shader = std::ranges::find_if(computePipelines, [name](const auto& pipeline) {return pipeline->getName() == name; });
		device.get()->getLogical().waitIdle();
		shader->get()->recreate(*device, *drawImage, *descriptorAllocator);
		device.get()->getLogical().waitIdle();
		});
}

std::shared_ptr<Imp::Render::Node> Imp::Render::VKRenderer::getLoadedGltfChildNode(const std::string& gltf,
	const std::string& child)
{
	if (!resourceHandler->hasGLTF(gltf)) {
		return nullptr;
	}
	const auto& node = resourceHandler->getGLTF(gltf);
	if (!node) {
		return nullptr;
	}
	//if (!childNode) {
	//	return nullptr;
	//}
	//loadedNodes[child] = childNode;
	return node->getNode(child);
}

const std::vector<std::string>& Imp::Render::VKRenderer::getLoadedGLTFNames()
{
	// TODO: insert return statement here
	return resourceHandler->getGLTFNames();
}

const std::vector<std::string>& Imp::Render::VKRenderer::getLoadedGLTFNodeNames(std::string gltfName)
{
	if (!resourceHandler->hasGLTF(gltfName)) {
		return {};
	}
	return resourceHandler->getGLTF(gltfName)->nodeNames;
}

const std::vector<std::string>& Imp::Render::VKRenderer::getMaterialNames()
{

	return resourceHandler->getMaterialNames();
}
