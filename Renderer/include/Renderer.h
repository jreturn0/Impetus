#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <string>

#include "gpudata/GPUSceneData.h"
#include "nodes/Node.h"
#include "utils/QuickMacros.h"

#include "utils/RenderStats.h"

namespace Imp {
	class Window;
	class ComputeEffect;
	namespace Render {

		;	class IRenderable;

		class Renderer
		{
		protected:
			uint32_t currentFrame = 0;
			uint64_t frameNumber = 0;
			uint32_t imageIndex = 0;
			bool isWindowResized = false;
			float renderScale = 1.f;
			GPUSceneData sceneData;
			std::array<float, 4> clearColour = { 0.0f, 0.0f, 0.0f, 1.0f };
			int currentComputePipeline{ 0 };

			virtual void recreate() = 0;

			RenderStats stats;
			bool culling = true;

			//virtual void drawCompute() = 0;
			//virtual void drawGeometry() = 0;
			////virtual void drawGui() = 0;


		public:
			Renderer();
			virtual ~Renderer();
			DISABLE_COPY_AND_MOVE(Renderer);


			//Getters
			virtual std::shared_ptr<Window> getSharedWindow() = 0;
			virtual Window& getWindow() = 0;
			uint32_t getCurrentFrame() const { return currentFrame; }
			uint64_t getFrameNumber() const { return frameNumber; }
			float getRenderScale() const { return renderScale; }
			void setRenderScale(float scale) { renderScale = scale; }
			GPUSceneData& getSceneData() { return sceneData; }
			const GPUSceneData& getSceneData() const { return sceneData; }
			void setSceneData(const GPUSceneData& data) { sceneData = data; }
			const RenderStats& getStats() const { return stats; }

			virtual const std::vector<std::string>& getLoadedGLTFNames() = 0;
			virtual const std::vector<std::string>& getLoadedGLTFNodeNames(std::string gltfName) = 0;
			virtual const std::vector<std::string>& getMaterialNames() = 0;

			void setCulling(bool cull) { culling = cull; }
			void toggleCulling() { culling = !culling; }
			bool isCulling() const { return culling; }
			virtual void reloadShader(const std::string& name) = 0;

			auto getClearColour() const { return clearColour; }
			void setClearColour(const std::array<float, 4>& colour) { clearColour = colour; }
			void setClearColour(float r, float g, float b, float a) { clearColour = { r, g, b, a }; }


			int getCurrentComputeIndex() const { return currentComputePipeline; }
			void setCurrentComputeIndex(int pipeline) { currentComputePipeline = pipeline; }
			virtual int getComputeEffectsSize() = 0;
			virtual ComputeEffect& getCurrentComputeEffect() = 0;


			virtual void windowResized()
			{
				isWindowResized = true;
			};


			virtual void draw(const ::std::string& name, const ::std::string& gltf, const glm::mat4& transform, const std::string& materialName) = 0;
			virtual void draw(const std::string& name, const std::string& gltf,const std::string& materialName, const glm::mat4& transform) = 0;
			virtual void draw(const std::string& gltf, const glm::mat4& transform) = 0;


			virtual void loadGLTF(std::filesystem::path filePath) = 0;
			virtual std::shared_ptr<Node> getLoadedGltfNode(const std::string& gltf, const std::string& node) = 0;
			virtual std::shared_ptr<Node> getLoadedGltfChildNode(const std::string& glft, const std::string& child) = 0;

			virtual void loadComputeShader(const std::string& name) = 0;
			virtual std::weak_ptr<ComputeEffect> getComputeEffect(const std::string& name) = 0;




			virtual void beginFrame() = 0;

			virtual void drawCompute() = 0;


			virtual void drawGeometry() = 0;


			virtual void startDrawGui() = 0;
			virtual void endDrawGui() = 0;

			virtual void endFrame() = 0;
		};


	}
}
