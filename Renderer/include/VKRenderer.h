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

    // Vulkan implementation of the Renderer interface
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




        // Mesthods
      
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

        void drawLight(GPULight light) override;

        void reloadComputeShader(std::string_view filename) override;
        void reloadAllShaders() override;

        void beginFrame() override;
        void drawCompute() override;
        void drawGeometry() override;
        void startDrawGui() override;
        void endDrawGui() override;
        void endFrame() override;
    private:
        SharedWindow m_window{nullptr};
        std::unique_ptr<VulkanContext> m_context{ nullptr };
        std::unique_ptr<SwapChain> m_swapChain{ nullptr };
        UniqueDescriptorAllocatorGrowable m_globalDescriptorAllocator{ nullptr };
        std::unique_ptr<ResourceCache> m_resourceCache{ nullptr };
        vk::raii::DescriptorSetLayout m_gpuSceneDataDescriptorLayout{ nullptr };
        std::array<UniqueFrameData, vkutil::MAX_FRAMES_IN_FLIGHT> m_frames{};
        UniqueImage m_drawImage{ nullptr };
        UniqueImage m_depthImage{ nullptr };
        UniqueGUI m_gui{ nullptr };
        UniqueMetallicRoughness m_metallicRoughnessPBR{ nullptr };
        SharedImage m_whiteImage{ nullptr };
        SharedImage m_greyImage{ nullptr };
        SharedImage m_blackImage{ nullptr };
        SharedImage m_errorImage{ nullptr };
        vk::raii::Sampler m_defaultSamplerLinear{ nullptr };
        vk::raii::Sampler m_defaultSamplerNearest{ nullptr };
        SharedMaterial m_defaultMaterial{ nullptr };
        std::unique_ptr< DrawContext > m_drawContext{ nullptr };
        std::vector<SharedComputePipeline> m_computePipelines{};
        vk::Extent2D m_drawExtent{};
        std::chrono::time_point<std::chrono::high_resolution_clock> m_frameStartTime = std::chrono::high_resolution_clock::now();
        std::queue < std::function<void()>> m_pendingFunctions{};
        uint32_t m_semaphoreIndex{};
        bool m_shaderReloadQueued{};

        // Private Methods

        FrameData& getCurrentFrameData() { return *m_frames[m_currentFrame]; }
        void recreate() override;
        SharedMaterial createDefaultMaterial();

    };
}
