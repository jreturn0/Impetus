#include "core/ComputeEffect.h"
#include "core/ComputePipeline.h"
#include "core/FrameData.h"
#include "core/Image.h"
#include "core/SwapChain.h"
#include "Debug.h"
#include "geometry//Mesh.h"
#include "gpudata/GPUDrawPushConstants.h"
#include "materials/MetallicRoughness.h"
#include "nodes/Node.h"
#include "rendercontext/DrawContext.h"
#include "rendercontext/RenderObject.h"
#include "TimerStats.h"
#include "utils/descriptor/DescriptorLayoutBuilder.h"
#include "utils/GUI.h"
#include "utils/LoadedGLTF.h"
#include "utils/MeshLoader.h"
#include "utils/ResourceCache.h"
#include "utils/shader/ShaderCompiler.h"
#include "utils/VKUtils.h"
#include "VKRenderer.h"
#include <algorithm>
#include <ConfigSystem.h>
#include <execution>
#include <glm/packing.hpp>
#include <memory>
#include <mutex>





using namespace utl::hashLiterals;


// Anonymous namespace for utility functions only used here
namespace {

    imp::gfx::UniqueImage CreateDrawImage(const imp::gfx::VulkanContext& context, vk::Extent2D extent)
    {
        //TODO: FIX the formatting, needs more than one option ideally
        return std::make_unique<imp::gfx::Image>(context, extent, imp::gfx::vkutil::PickDrawImageFormat(context.getPhysicalDevice()),
            (vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst |
                vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eColorAttachment),
            vk::ImageAspectFlagBits::eColor);

    }

    imp::gfx::UniqueImage CreateDepthImage(const imp::gfx::VulkanContext& context, vk::Extent2D extent)
    {
        return std::make_unique<imp::gfx::Image>(context, extent, imp::gfx::vkutil::PickDepthFormat(context.getPhysicalDevice()),
            (vk::ImageUsageFlagBits::eDepthStencilAttachment), vk::ImageAspectFlagBits::eDepth);
    }

    void SetViewportAndScissors(vk::CommandBuffer cmd, const vk::Extent2D extent)
    {
        vk::Viewport viewport(0.0f, 0.0f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f);
        cmd.setViewport(0, viewport);

        vk::Rect2D scissor({ 0, 0 }, extent);
        cmd.setScissor(0, scissor);
    }

    vk::RenderingAttachmentInfoKHR GetColorAttachment(vk::ImageView view, vk::AttachmentLoadOp op, vk::ClearColorValue clear = { 0.f,0.f,0.f,1.f })
    {
        return vk::RenderingAttachmentInfoKHR{}
            .setImageView(view)
            .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setLoadOp(op)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setClearValue(clear);
    }

    auto GetDepthAttachment(const vk::ImageView* view, vk::AttachmentLoadOp op, vk::ClearDepthStencilValue clear = { 1.f,0 })
    {
        return vk::RenderingAttachmentInfoKHR{}
            .setImageView(view != nullptr ? *view : vk::ImageView{})
            .setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal)
            .setLoadOp(op)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setClearValue(clear);
    }

    void SetBeginRendering(const vk::CommandBuffer cmd, const vk::Extent2D extent, const vk::ImageView colour, vk::AttachmentLoadOp loadOp, const vk::ImageView* depth = nullptr, vk::ClearColorValue clear = { 0.f,0.f,0.f,1.f })
    {
        auto colourAttachment = GetColorAttachment(colour, loadOp, clear);
        auto depthAttachment = depth != nullptr ? GetDepthAttachment(depth, vk::AttachmentLoadOp::eClear, { 1.f,0 }) : vk::RenderingAttachmentInfoKHR{};
        const auto renderingInfo = vk::RenderingInfoKHR{}
            .setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), extent))
            .setLayerCount(1)
            .setColorAttachmentCount(1)
            .setPColorAttachments(&colourAttachment)
            .setPDepthAttachment(depth != nullptr ? &depthAttachment : nullptr);
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
    template<typename T>
    concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

    template<TriviallyCopyable T>
    auto CreateColorImage(const imp::gfx::VulkanContext& context, imp::gfx::ResourceCache& resourceCache, vk::Format format, std::string_view name, const T& data)
    {
        auto bytes = std::as_bytes(std::span{ &data, 1 });
        auto image = imp::gfx::vkutil::CreateSampledSharedImage(context, { bytes, { 1,1 } }, format, vk::ImageUsageFlagBits::eSampled, false);
        resourceCache.addImage(name, image);
        return image;
    }

    template<TriviallyCopyable T, size_t N>
    auto CreateColorImage(const imp::gfx::VulkanContext& context, imp::gfx::ResourceCache& resourceCache, vk::Format format, std::string_view name, const std::array<T, N>& data)
    {
        auto bytes = std::as_bytes(std::span{ data });
        // Attempt to interpret as square texture if possible
        uint32_t side = static_cast<uint32_t>(std::sqrt(static_cast<double>(N)));
        vk::Extent2D extent{};
        if (side * side == N) {
            extent = vk::Extent2D{ side, side };
        }
        else {
            // Fallback: 1D strip
            extent = vk::Extent2D{ static_cast<uint32_t>(N), 1 };
        }

        auto image = imp::gfx::vkutil::CreateSampledSharedImage(context, { bytes, { extent.width,extent.height } }, format, vk::ImageUsageFlagBits::eSampled, false);
        resourceCache.addImage(name, image);
        return image;
    }

    auto CreateWhiteImage(const imp::gfx::VulkanContext& context, imp::gfx::ResourceCache& resourceCache, vk::Format format)
    {
        return CreateColorImage(context, resourceCache, format, "white_image", glm::packUnorm4x8(glm::vec4(1, 1, 1, 1)));
    }
    auto CreateGreyImage(const imp::gfx::VulkanContext& context, imp::gfx::ResourceCache& resourceCache, vk::Format format)
    {
        return CreateColorImage(context, resourceCache, format, "grey_image", glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1)));
    }
    auto CreateBlackImage(const imp::gfx::VulkanContext& context, imp::gfx::ResourceCache& resourceCache, vk::Format format)
    {
        return CreateColorImage(context, resourceCache, format, "black_image", glm::packUnorm4x8(glm::vec4(0, 0, 0, 0)));
    }
    auto CreateErrorImage(const imp::gfx::VulkanContext& context, imp::gfx::ResourceCache& resourceCache, vk::Format format)
    {
        uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
        uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
        constexpr size_t checkerSize = 16;
        std::array<uint32_t, checkerSize* checkerSize> pixels{}; //for 16x16 checkerboard texture
        for (int x = 0; x < checkerSize; x++) {
            for (int y = 0; y < checkerSize; y++) {
                pixels[y * checkerSize + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
            }
        }
        return CreateColorImage(context, resourceCache, format, "error_image", pixels);
    }

    auto CreateGPUSceneDescriptorLayout(const vk::raii::Device& device)
    {
        imp::gfx::DescriptorLayoutBuilder builder;

        builder.addBinding(0, vk::DescriptorType::eUniformBuffer); // Scene UBO
        builder.addBinding(1, vk::DescriptorType::eStorageBuffer); // Light SSBO

        return builder.build(device, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    }



    bool isBoundingBoxVisible(const imp::gfx::RenderObject& obj, const glm::mat4& viewProj)
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
    vk::raii::Sampler CreateDefaultSampler(const vk::raii::Device& device, vk::Filter filter)
    {
        vk::SamplerCreateInfo samplerInfo{ {},filter,filter };
        return device.createSampler(samplerInfo);
    }
}


void imp::gfx::VKRenderer::recreate()
{

    auto& device = m_context->getDevice();

    vkutil::CheckResult(device.waitForFences(m_frames[m_currentFrame]->getInFlightFence(), vk::True, UINT64_MAX));
    m_swapChain->recreateSwapChain(*m_context, *m_window);
    m_drawImage = CreateDrawImage(*m_context, m_swapChain->getExtent());
    m_depthImage = CreateDepthImage(*m_context, m_swapChain->getExtent());
    for (auto& compute : m_computePipelines) {
        compute->recreate(device, *m_drawImage, *m_globalDescriptorAllocator);
    }
}

imp::gfx::SharedMaterial imp::gfx::VKRenderer::createDefaultMaterial()
{
    auto defaultImage = m_resourceCache->getImage(vkutil::GREY_IMAGE_HASH);
    auto defaultSampler = m_resourceCache->getDefaultSamplerLinear();

    MetallicRoughness::MaterialResources materialResources;
    //default the material textures
    materialResources.colorImage = defaultImage;
    materialResources.colorSampler = defaultSampler;
    materialResources.metalRoughImage = materialResources.colorImage;
    materialResources.metalRoughSampler = materialResources.colorSampler;

    auto materialConstants = std::make_shared<Buffer>(m_context->getVmaAllocator(), sizeof(MetallicRoughness::MaterialConstants), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);;
    //write the buffer
    MetallicRoughness::MaterialConstants* sceneUniformData = static_cast<MetallicRoughness::MaterialConstants*>(materialConstants->getAllocationInfo().pMappedData);
    sceneUniformData->colorFactors = glm::vec4{ 1,1,1,1 };
    sceneUniformData->metalRoughFactors = glm::vec4{ 1,0.5,0,0 };



    materialResources.dataBuffer = std::move(materialConstants);
    materialResources.dataOffset = 0;
    materialConstants = nullptr;
    return m_metallicRoughnessPBR->writeMaterial(m_context->getDevice(), MaterialPass::Opaque, materialResources, *m_globalDescriptorAllocator);
}

void imp::gfx::VKRenderer::reloadAllShaders()
{
    // Coalesce if a reload is already queued
    if (m_shaderReloadQueued) {
        Debug::Info("Shader reload already queued.");
        return;
    }
    m_shaderReloadQueued = true;
    Debug::Info("Queueing shader reload...");

    m_pendingFunctions.emplace([this]() {
        Debug::Info("Reloading all shaders (deferred)...");
        ShaderCompiler::Instance().CheckCompileAll();
        auto& device = m_context->getDevice();
        device.waitIdle();
        try {
            for (auto& compute : m_computePipelines) {
                compute->recreate(device, *m_drawImage, *m_globalDescriptorAllocator);
            }
            m_metallicRoughnessPBR->recreate(device, *m_gpuSceneDataDescriptorLayout,
                m_drawImage->getFormat(), m_depthImage->getFormat());
            m_resourceCache->forEachMaterial([this](imp::gfx::Material& material) {
                m_metallicRoughnessPBR->updateMaterialPipelines(material);
                });

            Debug::Info("Shader reload completed.");
        }
        catch (const std::exception& e) {
            Debug::Exception("Failed to reload shaders: {}", e.what());
        }
        m_shaderReloadQueued = false;
        });
}


imp::gfx::VKRenderer::VKRenderer(std::string_view windowTitle) :
    m_window( std::make_shared<Window>(windowTitle)),
    m_context(std::make_unique<VulkanContext>(*m_window)),
    m_swapChain(std::make_unique<SwapChain>(*m_context, *m_window)),
    m_globalDescriptorAllocator(std::make_unique<DescriptorAllocatorGrowable>(m_context->getDevice(), 10, std::vector<DescriptorAllocatorGrowable::PoolSizeRatio>{
        { vk::DescriptorType::eUniformBuffer, 3.0f },
        { vk::DescriptorType::eCombinedImageSampler, 4.0f },
        { vk::DescriptorType::eSampledImage,         4.0f },
        { vk::DescriptorType::eStorageImage,         2.0f }})),
    m_resourceCache(std::make_unique<ResourceCache>(m_context->getDevice())),
    m_gpuSceneDataDescriptorLayout(CreateGPUSceneDescriptorLayout(m_context->getDevice())),
    m_frames(CreateFrameDataArray(m_context->getDevice(), m_context->getVmaAllocator(), *m_swapChain, m_context->getGraphicsCommandPool(),m_gpuSceneDataDescriptorLayout)),
    m_drawImage(CreateDrawImage(*m_context, m_swapChain->getExtent())),
    m_depthImage(CreateDepthImage(*m_context, m_swapChain->getExtent())),
    m_gui(std::make_unique<GUI>(*m_context, *m_window, imp::gfx::vkutil::PickDrawImageFormat(m_context->getPhysicalDevice()))),
    m_metallicRoughnessPBR(std::make_unique<MetallicRoughness>(m_context->getDevice(), m_gpuSceneDataDescriptorLayout, m_drawImage->getFormat(), m_depthImage->getFormat())),
    m_whiteImage(CreateWhiteImage(*m_context, *m_resourceCache, m_swapChain->getSurfaceFormat().format)),
    m_greyImage(CreateGreyImage(*m_context, *m_resourceCache, m_swapChain->getSurfaceFormat().format)),
    m_blackImage(CreateBlackImage(*m_context, *m_resourceCache, m_swapChain->getSurfaceFormat().format)),
    m_errorImage(CreateErrorImage(*m_context, *m_resourceCache, m_swapChain->getSurfaceFormat().format)),
    m_defaultSamplerLinear(CreateDefaultSampler(m_context->getDevice(), vk::Filter::eLinear)),
    m_defaultSamplerNearest(CreateDefaultSampler(m_context->getDevice(), vk::Filter::eNearest)),
    m_defaultMaterial(createDefaultMaterial()),
    m_drawContext(std::make_unique<DrawContext>())
{
    MetallicRoughness::MaterialResources materialResources = MetallicRoughness::MaterialResources{
    m_errorImage,
    *m_defaultSamplerNearest,
    m_errorImage,
    *m_defaultSamplerNearest,
    std::make_shared<Buffer>(m_context->getVmaAllocator(),
                       sizeof(MetallicRoughness::MaterialConstants),
                       vk::BufferUsageFlagBits::eUniformBuffer,
                       VMA_MEMORY_USAGE_CPU_TO_GPU),0
    };

    auto errorMaterial = m_metallicRoughnessPBR->writeMaterial(m_context->getDevice(), MaterialPass::Opaque, materialResources
        , m_resourceCache->getMaterialDescriptorAllocator());
    bool added = m_resourceCache->addMaterial("error_material", errorMaterial);
    if (!added) {
        Debug::Error("Failed to add error material to resource cache");
    }

}






imp::gfx::VKRenderer::~VKRenderer()
{
    m_context->getDevice().waitIdle();
}



std::shared_ptr<imp::Window> imp::gfx::VKRenderer::getSharedWindow()
{
    return m_window;
}

imp::Window& imp::gfx::VKRenderer::getWindow()
{
    return *m_window;
}

void imp::gfx::VKRenderer::loadGLTF(std::filesystem::path filePath)
{

    if (m_resourceCache->hasGLTF(utl::StringHash(filePath.string()))) {
        return;
    }

    auto gltf = MeshLoader::LoadGltf(filePath.string(), *m_context, *m_resourceCache, *m_metallicRoughnessPBR);
    if (!gltf.has_value()) {
        Debug::Error("Failed to load gltf file");
        return;
    }
    bool added = m_resourceCache->addGLTF(filePath.filename().string(), gltf.value());
    if (!added) {
        Debug::Error("Failed to add gltf to resource cache");
    }
}

std::shared_ptr<imp::gfx::Node> imp::gfx::VKRenderer::getLoadedGltfNode(std::string_view gltfFilename, std::string_view nodeName)
{
    return m_resourceCache->getGLTF(gltfFilename)->getNode(nodeName);
}

void imp::gfx::VKRenderer::loadComputeShader(std::filesystem::path filePath)
{
    m_computePipelines.push_back(std::make_unique<ComputePipeline>(m_context->getDevice(), *m_drawImage, *m_globalDescriptorAllocator, filePath.filename().string()));

}

std::weak_ptr<imp::ComputeEffect> imp::gfx::VKRenderer::getComputeEffect(std::string_view name)
{
    auto result = std::ranges::find_if(m_computePipelines, [name](const auto& pipeline) {return pipeline->getName() == name; });
    if (result == m_computePipelines.end()) {
        return {};
    }
    return (*result);
}


void imp::gfx::VKRenderer::beginFrame()
{
    m_frameStartTime = std::chrono::high_resolution_clock::now();
    RENDERSTATS_SCOPED_TIMER(m_stats, "beginFrame");
    auto& frame = getCurrentFrameData();
    const auto& cmd = frame.getCommandBuffer();
    const auto& device = m_context->getDevice();
    m_drawExtent.setHeight(static_cast<uint32_t>(std::min(m_swapChain->getExtent().height, m_drawImage->getExtent().height) * m_renderScale))
        .setWidth(static_cast<uint32_t>(std::min(m_swapChain->getExtent().width, m_drawImage->getExtent().width) * m_renderScale));
    try {
        device.waitForFences(frame.getInFlightFence(), vk::True, std::numeric_limits<uint64_t>::max());
    }
    catch (const vk::SystemError& e) {
        Debug::Exception("Error waiting for fence in beginFrame: {}", e.what());
    }
    auto [result, imageIndex] = m_swapChain->getSwapChain().acquireNextImage(std::numeric_limits<uint64_t>::max(), m_swapChain->getPresentCompleteSemaphore(m_semaphoreIndex), nullptr);
    m_imageIndex = imageIndex;
    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreate();
        m_isWindowResized = false;
        return;
    }
    else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }
    device.resetFences(frame.getInFlightFence());
    frame.getCommandBuffer().reset();

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
    if (cmd.begin(&beginInfo) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }
}

void imp::gfx::VKRenderer::drawCompute()
{
    RENDERSTATS_SCOPED_TIMER(m_stats, "drawCompute");
    m_drawImage->transitionImageLayout(getCurrentFrameData().getCommandBuffer(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
    if (m_computePipelines.empty()) {
        return;
    }
    auto cmd = getCurrentFrameData().getCommandBuffer();
    m_computePipelines[m_currentComputePipeline]->bind(cmd);
    auto [width, height] = m_drawImage->getExtent();
    width = static_cast<uint32_t>(std::ceil(width * m_renderScale));
    height = static_cast<uint32_t>(std::ceil(height * m_renderScale));
    m_computePipelines[m_currentComputePipeline]->dispatch(cmd, width, height, 1);
}



void imp::gfx::VKRenderer::drawGeometry()
{
    RENDERSTATS_SCOPED_TIMER(m_stats, "drawGeometry");
    m_stats.drawCallCount = 0;
    m_stats.triangleCount = 0;
    auto& frameData = getCurrentFrameData();
    auto cmd = frameData.getCommandBuffer();
    {
        RENDERSTATS_SCOPED_TIMER(m_stats, "transitionAndBeginRendering");
        m_drawImage->transitionImageLayout(cmd, vk::ImageLayout::eGeneral, vk::ImageLayout::eColorAttachmentOptimal);
        m_depthImage->transitionImageLayout(cmd, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal);
        vk::ImageView depthImg = m_depthImage->getView();
        SetBeginRendering(cmd, m_drawExtent, m_drawImage->getView(), vk::AttachmentLoadOp::eDontCare, &depthImg);
    }

    std::vector<size_t> opaque_draws;
    opaque_draws.reserve(m_drawContext->opaque.size());
    {
        RENDERSTATS_SCOPED_TIMER(m_stats, "visibilityCheck");
        std::mutex drawMutex;
        std::for_each(std::execution::par, m_drawContext->opaque.begin(), m_drawContext->opaque.end(), [&](const RenderObject& obj) {
            if (!m_culling) {
                std::lock_guard<std::mutex> lock(drawMutex);
                opaque_draws.push_back(&obj - m_drawContext->opaque.data());
                return;
            }

            if (isBoundingBoxVisible(obj, m_sceneData.viewproj)) {
                std::lock_guard<std::mutex> lock(drawMutex);
                opaque_draws.push_back(&obj - m_drawContext->opaque.data());
            }
            });
    }
    {
        RENDERSTATS_SCOPED_TIMER(m_stats, "sort");
        std::ranges::sort(opaque_draws, [&](const auto& iA, const auto& iB) {
            const RenderObject& A = m_drawContext->opaque[iA];
            const RenderObject& B = m_drawContext->opaque[iB];
            if (A.material == B.material) {
                return A.indexBuffer < B.indexBuffer;
            }
            else {
                return A.material < B.material;
            }
            });
    }

    {
        RENDERSTATS_SCOPED_TIMER(m_stats, "updateSceneData");
        auto& gpuSceneDataBuffer = frameData.getSceneDataBuffer();

        GPUSceneData* sceneUniformData = static_cast<GPUSceneData*>(gpuSceneDataBuffer.getAllocationInfo().pMappedData);
        *sceneUniformData = m_sceneData;
        size_t index = 0;
        for (auto& light : m_drawContext->lights) {
            if (index >= GPULightData::MaxLights) {
                break;
            }
            m_lightData.lights[index] = light;
            ++index;         
        }
        m_drawContext->lights.clear();
        m_lightData.lightCount = static_cast<uint32_t>(index);
        auto& gpuLightDataBuffer = frameData.getLightDataBuffer();
        GPULightData* lightData = static_cast<GPULightData*>(gpuLightDataBuffer.getAllocationInfo().pMappedData);
        *lightData = m_lightData;
        DescriptorWriter writer{};
        // Correct order: size then offset
        writer.writeBuffer(0, frameData.getSceneDataBuffer().getBuffer(), sizeof(GPUSceneData), 0, vk::DescriptorType::eUniformBuffer);
        writer.writeBuffer(1, gpuLightDataBuffer.getBuffer(), sizeof(GPULightData), 0, vk::DescriptorType::eStorageBuffer);
        writer.updateSet(m_context->getDevice(), frameData.getSceneDataDescriptorSet());

    }
    {
        RENDERSTATS_SCOPED_TIMER(m_stats, "drawingGeo");
        vk::Pipeline lastPipeline{ nullptr };
        std::shared_ptr<Material> lastMaterial = nullptr;
        size_t pipelineSwitches = 0;
        vk::Buffer lastIndexBuffer = VK_NULL_HANDLE;

        auto draw = [&](const RenderObject& robj) {
            if (robj.material != lastMaterial) {
                lastMaterial = robj.material;

                if (robj.material->pipeline != lastPipeline) {
                    //	std::cout << "swapping pipelines\n";
                    lastPipeline = robj.material->pipeline;
                    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, robj.material->pipeline);
                    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, robj.material->pipelineLayout, 0, frameData.getSceneDataDescriptorSet(), {});
                    SetViewportAndScissors(cmd, m_drawExtent);
                }
                cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, robj.material->pipelineLayout, 1, *robj.material->set, {});
            }
            if (robj.indexBuffer != lastIndexBuffer) {
                lastIndexBuffer = robj.indexBuffer;
                cmd.bindIndexBuffer(robj.indexBuffer, 0, vk::IndexType::eUint32);
            }
            GPUDrawPushConstants pushConstants{ robj.transform, robj.vertexBufferAddress };
            cmd.pushConstants<GPUDrawPushConstants>(robj.material->pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, pushConstants);
            cmd.drawIndexed(robj.indexCount, 1, robj.firstIndex, 0, 0);
            ++m_stats.drawCallCount;
            m_stats.triangleCount += robj.indexCount / 3;
            };

        for (auto& r : opaque_draws) {
            draw(m_drawContext->opaque[r]);
        }
        m_drawContext->opaque.clear();

        for (auto&& mesh : m_drawContext->transparent) {
            draw(mesh);
        }
        m_drawContext->transparent.clear();

        cmd.endRendering();
    }
}



void imp::gfx::VKRenderer::startDrawGui()
{
    m_gui->startFrame();
}

void imp::gfx::VKRenderer::endDrawGui()
{
    RENDERSTATS_SCOPED_TIMER(m_stats, "drawGui");;
    m_gui->endFrame();
    //auto start = std::chrono::high_resolution_clock::now();
    const vk::CommandBuffer& cmd = m_frames[m_currentFrame]->getCommandBuffer();
    SetBeginRendering(cmd, m_drawImage->getExtent(), m_drawImage->getView(), vk::AttachmentLoadOp::eDontCare);
    SetViewportAndScissors(cmd, m_drawImage->getExtent());
    m_gui->render(cmd);
    cmd.endRendering();
}
std::shared_ptr<imp::gfx::Mesh> imp::gfx::VKRenderer::uploadMesh(const std::string& name,
    std::span<GeoSurface> surfaces,
    std::span<uint32_t> indices,
    std::span<Vertex> vertices) const
{
    return std::make_shared<Mesh>(name, *m_context, surfaces, indices, vertices);
}

void imp::gfx::VKRenderer::endFrame()
{
    RENDERSTATS_SCOPED_TIMER(m_stats, "endFrame");
    const vk::CommandBuffer& cmd = m_frames[m_currentFrame]->getCommandBuffer();
    auto& frame = m_frames[m_currentFrame];

    m_drawImage->transitionImageLayout(cmd, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal);
    vkutil::TransitionImageLayout(cmd, m_swapChain->getImages()[m_imageIndex], vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    vkutil::CopyImageToSwapChain(cmd, *m_drawImage, *m_swapChain, m_imageIndex);
    vkutil::TransitionImageLayout(cmd, m_swapChain->getImages()[m_imageIndex], vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR);
    cmd.end();
    // Submit the command buffer for execution
    vk::PipelineStageFlags2 waitStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    vk::SemaphoreSubmitInfo waitSemaphoreInfo(m_swapChain->getPresentCompleteSemaphore(m_semaphoreIndex), 0, waitStage);
    vk::CommandBufferSubmitInfo commandBufferInfo(frame->getCommandBuffer());

    vk::SemaphoreSubmitInfo signalSemaphoreInfo(m_swapChain->getRenderFinishedSemaphore(m_imageIndex), 0, vk::PipelineStageFlagBits2::eBottomOfPipe);
    vk::SubmitInfo2 submitInfo2({}, waitSemaphoreInfo, commandBufferInfo, signalSemaphoreInfo);

    m_context->getGraphicsQueue().submit2(submitInfo2, frame->getInFlightFence());

    auto renderFinishedSemaphore = m_swapChain->getRenderFinishedSemaphore(m_imageIndex);
    vk::PresentInfoKHR presentInfo{ renderFinishedSemaphore,*m_swapChain->getSwapChain(),m_imageIndex };


    vk::Queue queue = m_context->getGraphicsQueue();
    vk::Result result = queue.presentKHR(&presentInfo);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        recreate();
        m_isWindowResized = false;
    }
    else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swap chain image!");
    }
    ++m_frameNumber;
    m_currentFrame = (m_currentFrame + 1u) % static_cast<uint32_t>(imp::gfx::vkutil::MAX_FRAMES_IN_FLIGHT);
    m_semaphoreIndex = (m_semaphoreIndex + 1u) % static_cast<uint32_t>(m_swapChain->getImageCount());
    getCurrentFrameData().flushDeletions();

    while (!m_pendingFunctions.empty()) {
        m_pendingFunctions.front()();
        m_pendingFunctions.pop();
    }
    auto frameEndTime = std::chrono::high_resolution_clock::now();
    m_stats.timeStatsMap["totalFrame"].update(std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - m_frameStartTime).count());
}

imp::ComputeEffect& imp::gfx::VKRenderer::getCurrentComputeEffect()
{
    return *m_computePipelines[m_currentComputePipeline];
}

int imp::gfx::VKRenderer::getComputeEffectsSize()
{
    return static_cast<int>(m_computePipelines.size());
}
void imp::gfx::VKRenderer::draw(std::string_view gltfFilename, std::string_view nodeName, const glm::mat4& transform, std::string_view materialName)
{
    if (!m_resourceCache->hasGLTF(gltfFilename)) {
        if (!gltfFilename.empty())
            Debug::Warning("GLTF ( {} ) does not exist", gltfFilename);
        return;
    }
    const auto& gltfNode = m_resourceCache->getGLTF(gltfFilename);
    if (!gltfNode) {
        Debug::Warning("GLTF ( {} ) failed to load", gltfFilename);
        return;
    }
    if (nodeName.empty() || nodeName == "Root") {
        gltfNode->draw(transform, *m_drawContext);
        return;
    }
    if (auto&& node = gltfNode->getNode(nodeName); node != nullptr) {
        // Draw the node with or without a specific material
        if (materialName.empty() || materialName == "default") {
            node->draw(transform, *m_drawContext);
            return;
        }
        if (m_resourceCache->hasMaterial(materialName)) {
            node->draw(transform, *m_drawContext, m_resourceCache->getMaterialRef(materialName));
            return;
        }
        Debug::Warning("Material ( {} ) not found, using error material", materialName);
        node->draw(transform, *m_drawContext, m_resourceCache->getMaterialRef("error_material"));
        return;
    }
    Debug::Warning("GLTF ( {} ) node with name ( {} ) does not exist", gltfFilename, nodeName);
}

void imp::gfx::VKRenderer::draw(std::string_view gltfFilename, const glm::mat4& transform)
{
    draw(gltfFilename, "Root", transform, "");
}

void imp::gfx::VKRenderer::drawLight(GPULight light)
{
    m_drawContext->lights.push_back(light);
}

void imp::gfx::VKRenderer::draw(std::string_view nodeName, std::string_view gltfFilename, std::string_view materialName, const glm::mat4& transform)
{
    draw(gltfFilename, nodeName, transform, materialName);
}


void imp::gfx::VKRenderer::windowResized()
{
    Renderer::windowResized();
    m_window->onResize();
}

void imp::gfx::VKRenderer::reloadComputeShader(std::string_view filename)
{
    using namespace utl::hashLiterals;
    //TODO: this may break, I'm not sure if the string_view will be valid when the deferred function is called
    m_pendingFunctions.emplace([this, filename]() {
        ShaderCompiler::Instance().CheckCompileAll(utl::ConfigSystem::instance().getGlobalConfigFile().getValueCopyOrDefault<std::string>("assets.shader_directory"_hash, "assets/shaders"));
        auto shader = std::ranges::find_if(m_computePipelines, [filename](const auto& pipeline) {return pipeline->getName() == filename; });
        m_context->getDevice().waitIdle();
        shader->get()->recreate(m_context->getDevice(), *m_drawImage, *m_globalDescriptorAllocator);
        m_context->getDevice().waitIdle();
        });
}

std::shared_ptr<imp::gfx::Node> imp::gfx::VKRenderer::getLoadedGltfChildNode(std::string_view gltfFilename, std::string_view childNodeName)
{
    if (!m_resourceCache->hasGLTF(gltfFilename)) {
        Debug::Warning("GLTF ( {} ) does not exist", gltfFilename);
        return nullptr;
    }
    const auto& node = m_resourceCache->getGLTF(gltfFilename);
    if (!node) {
        Debug::Warning("GLTF ( {} ) failed to load", gltfFilename);
        return nullptr;
    }
    return node->getNode(childNodeName);
}

const std::vector<std::string>& imp::gfx::VKRenderer::getLoadedGLTFNames()
{
    return m_resourceCache->getGLTFNames();
}

const std::vector<std::string>& imp::gfx::VKRenderer::getLoadedGLTFNodeNames(std::string_view gltfFilename)
{
    if (!m_resourceCache->hasGLTF(gltfFilename)) {
        static const std::vector<std::string>& emptyReturn{};
        if (!gltfFilename.empty())
            Debug::Warning("GLTF ( {} ) does not exist", gltfFilename);
        return emptyReturn;
    }
    return m_resourceCache->getGLTF(gltfFilename)->nodeNames;
}

const std::vector<std::string>& imp::gfx::VKRenderer::getMaterialNames()
{
    return m_resourceCache->getMaterialNames();
}

const std::vector<std::string>& imp::gfx::VKRenderer::getTextureNames()
{
    return m_resourceCache->getImageNames();
}
