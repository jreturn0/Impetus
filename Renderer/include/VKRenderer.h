#pragma once
#include <filesystem>
#include <functional>
#include <queue>
#include <unordered_map>

#include "Renderer.h"

#include "utils/Forwards.h"
#include "geometry/GeoSurface.h"
#include "geometry/Vertex.h"
#include "materials/MetallicRoughness.h"
#include "core/VulkanContext.h"
#include "core/Image.h"

namespace imp::gfx
{
    enum class MaterialPass : uint8_t;
    class ResourceCache;
}

namespace imp::gfx {
    struct LoadedGLTF;

    //using UniqueCommandBuffer = std::unique_ptr<class CommandBuffer>;

    class VKRenderer final : public Renderer
    {
    public:
        VKRenderer(std::string_view windowTitle);
        ~VKRenderer() override;
        VKRenderer(const VKRenderer&) = delete; 
        VKRenderer& operator=(const VKRenderer&) = delete; 
        VKRenderer(VKRenderer&&) = delete; 
        VKRenderer& operator=(VKRenderer&&) = delete;

        // Getters

        const VulkanContext& getContext() const { return *m_context; }
        const SwapChain& getSwapChain() const { return *m_swapChain; }
        const Image& getDrawImage() const { return *m_drawImage; }
        const Image& getDepthImage() const { return *m_depthImage; }
        const SharedImage& getWhiteImage() const { return m_whiteImage; }
        const SharedImage& getGreyImage() const { return m_greyImage; }
        const SharedImage& getBlackImage() const { return m_blackImage; }
        const SharedImage& getErrorCheckerboardImage() const { return m_errorImage; }
        vk::DescriptorSetLayout getGPUSceneDataDescriptorLayout() const { return *m_gpuSceneDataDescriptorLayout; }
        vk::Sampler getDefaultSamplerLinear() const { return *m_defaultSamplerLinear; }
        vk::Sampler getDefaultSamplerNearest() const { return *m_defaultSamplerNearest; }
        MetallicRoughness& getMetallicRoughness() const { return *m_metallicRoughnessPBR; }
        ResourceCache& getResourceCache() const { return *m_resourceCache; }





      
        std::shared_ptr<imp::gfx::Mesh> uploadMesh(const std::string& name, std::span<GeoSurface> surfaces, std::span<uint32_t> indices, std::span<Vertex> vertices) const;
        
        // Overloaded Getters

        Window&                         getWindow() override;
        std::shared_ptr<Window>         getSharedWindow() override;
        const std::vector<std::string>& getLoadedGLTFNames() override;
        const std::vector<std::string>& getLoadedGLTFNodeNames(std::string_view gltfFilename) override;
        const std::vector<std::string>& getMaterialNames() override;
        const std::vector<std::string>& getTextureNames() override;
        std::shared_ptr<Node>           getLoadedGltfNode(std::string_view gltfFilename, std::string_view nodeName) override;
        std::shared_ptr<Node>           getLoadedGltfChildNode(std::string_view glftFilename, std::string_view childNodeName) override;
        std::weak_ptr<ComputeEffect>    getComputeEffect(std::string_view name) override;
        int                             getComputeEffectsSize() override;
        ComputeEffect&                  getCurrentComputeEffect() override;

        // Overloaded Functions
        void windowResized() override;


        void loadComputeShader(std::filesystem::path filePath) override;
        void loadGLTF(std::filesystem::path filePath) override;

        void draw(std::string_view nodeName, std::string_view gltfFilename, const glm::mat4& transform, std::string_view materialName = "") override;
        void draw(std::string_view nodeName, std::string_view gltfFilename, std::string_view materialName, const glm::mat4& transform) override;
        void draw(std::string_view gltfFilename, const glm::mat4& transform) override;

        void reloadShader(std::string_view filename) override;

        void beginFrame() override;
        void drawCompute() override;
        void drawGeometry() override;
        void startDrawGui() override;
        void endDrawGui() override;
        void endFrame() override;
    private:
        //Core:

        SharedWindow m_window;
        std::unique_ptr<VulkanContext> m_context;
        std::unique_ptr<SwapChain> m_swapChain;
        UniqueDescriptorAllocatorGrowable m_globalDescriptorAllocator;
        std::unique_ptr<ResourceCache> m_resourceCache;
        //Per Frame:

        std::array<UniqueFrameData, vkutil::MAX_FRAMES_IN_FLIGHT> m_frames;
        //Images:

        UniqueImage m_drawImage;
        UniqueImage m_depthImage;
        //GUI:

        UniqueGUI m_gui;

        //Default Data:

        vk::raii::DescriptorSetLayout m_gpuSceneDataDescriptorLayout;
        UniqueMetallicRoughness m_metallicRoughnessPBR;
        SharedImage m_whiteImage;
        SharedImage m_greyImage;
        SharedImage m_blackImage;
        SharedImage m_errorImage;
        vk::raii::Sampler m_defaultSamplerLinear;
        vk::raii::Sampler m_defaultSamplerNearest;
        SharedMaterial m_defaultMaterial;
        //Draw Context:

        std::unique_ptr< DrawContext > m_drawContext;
        //Compute Pipelines:

        std::vector<SharedComputePipeline> m_computePipelines;
        //Renderer Data:

        vk::Extent2D m_drawExtent{ 0,0 };

        std::chrono::time_point<std::chrono::high_resolution_clock> m_frameStartTime = std::chrono::high_resolution_clock::now();

        std::queue < std::function<void()>> m_pendingFunctions;



        FrameData& getCurrentFrameData() { return *m_frames[m_currentFrame]; }

        void recreate() override;

        SharedMaterial createDefaultMaterial();

    public:
    };
}
