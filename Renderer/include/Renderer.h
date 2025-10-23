#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <string>

#include "gpudata/GPUSceneData.h"
#include "nodes/Node.h"
#include "utils/QuickMacros.h"

#include "utils/RenderStats.h"
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <string_view>
#include <vector>
#include <gpudata/GPULight.h>

namespace imp {
    class Window;
    class ComputeEffect;
    namespace gfx {
        class IRenderable;


        // Abstract base class for a rendering engine
        // Provides an interface for rendering operations, managing scene data, and handling compute effects.
        // Hides implementation details for different rendering backends.
        class Renderer
        {
        public:
            using Color = std::array<float, 4>;
            virtual ~Renderer();
            Renderer();
            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;
            Renderer(Renderer&&) = delete;
            Renderer& operator=(Renderer&&) = delete;


            // Getters

            uint32_t getCurrentFrame() const { return m_currentFrame; }
            uint64_t getFrameNumber() const { return m_frameNumber; }
            float getRenderScale() const { return m_renderScale; }
            const GPUSceneData& getSceneData() const { return m_sceneData; }
            GPUSceneData& getSceneData() { return m_sceneData; }
            const RenderStats& getStats() const { return m_stats; }
            int getCurrentComputeIndex() const { return m_currentComputePipeline; }
            bool isCulling() const { return m_culling; }
            Color getClearColour() const { return m_clearColor; }

            // Setters

            void setCurrentComputeIndex(int pipeline) { m_currentComputePipeline = pipeline; }
            void setRenderScale(float scale) { m_renderScale = scale; }
            void setSceneData(const GPUSceneData& data) { m_sceneData = data; }
            void setCulling(bool cull) { m_culling = cull; }
            void setClearColour(const Color& color) { m_clearColor = color; }
            void setClearColour(float r, float g, float b, float a) { m_clearColor = { r, g, b, a }; }

            // Toggle face culling on or off.
            void toggleCulling() { m_culling = !m_culling; }

            // Virtual Getters

            virtual Window& getWindow() = 0;
            virtual std::shared_ptr<Window>         getSharedWindow() = 0;
            virtual const std::vector<std::string>& getLoadedGLTFNames() = 0;
            virtual const std::vector<std::string>& getLoadedGLTFNodeNames(std::string_view gltfFilename) = 0;
            virtual const std::vector<std::string>& getMaterialNames() = 0;
            virtual const std::vector<std::string>& getTextureNames() = 0;
            virtual std::shared_ptr<Node>           getLoadedGltfNode(std::string_view gltfFilename, std::string_view nodeName) = 0;
            virtual std::shared_ptr<Node>           getLoadedGltfChildNode(std::string_view glftFilename, std::string_view childNodeName) = 0;
            virtual std::weak_ptr<ComputeEffect>    getComputeEffect(std::string_view name) = 0;
            virtual int getComputeEffectsSize() = 0;
            virtual ComputeEffect& getCurrentComputeEffect() = 0;

            // Virtual Functions

            // 

            // Loads a compute shader by its file name.
            virtual void loadComputeShader(std::filesystem::path filePath) = 0;

            // Loads a GLTF file from the specified file path. 
            virtual void loadGLTF(std::filesystem::path filePath) = 0;

            //Callback to be invoked when the window is resized.
            virtual void windowResized() { m_isWindowResized = true; };

            // Draws a node from a loaded GLTF model with a specified transformation matrix and optional material override.
            virtual void draw(std::string_view nodeName, std::string_view gltfFilename, const glm::mat4& transform, std::string_view materialName = "") = 0;

            // Draws a node from a loaded GLTF model with a specified transformation matrix and optional material override.
            virtual void draw(std::string_view nodeName, std::string_view gltfFilename, std::string_view materialName, const glm::mat4& transform) = 0;

            // Draw a loaded GLTF model from its root node with a specified transformation matrix.
            virtual void draw(std::string_view gltf, const glm::mat4& transform) = 0;

            // Draw a light source
            virtual void drawLight(GPULight light) = 0;

            // Reload and recompile a shader by its name, recreating any pipelines
            virtual void reloadComputeShader(std::string_view name) = 0;

            // Reload and recompile all shaders, recreating any pipelines

            virtual void reloadAllShaders() = 0;

            // Begins a new frame in the rendering sequence.
            virtual void beginFrame() = 0;

            // Draws using the current compute pipeline.
            virtual void drawCompute() = 0;

            // Draws the geometry of the scene.
            virtual void drawGeometry() = 0;

            // Begins the GUI rendering process. 
            virtual void startDrawGui() = 0;

            // Ends the GUI rendering process.
            virtual void endDrawGui() = 0;

            // Ends the current frame in the rendering sequence and presents it to the display.
            virtual void endFrame() = 0;
        protected:
            // Recreate the swap chain and related resources.
            virtual void recreate() = 0;

            uint64_t m_frameNumber{ 0 };
            uint32_t m_currentFrame{ 0 };
            uint32_t m_imageIndex{ 0 };
            uint32_t m_currentComputePipeline{ 0 };
            float m_renderScale{ 1.f };
            Color m_clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
            RenderStats m_stats;
            GPUSceneData m_sceneData;
            GPULightData m_lightData{};
            bool m_isWindowResized{ false };
            bool m_culling{ true };
        };


    }
}
