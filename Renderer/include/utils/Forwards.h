#pragma once

#include <memory>

class ComputeEffect;
namespace imp {
	class Window;
	using SharedWindow = std::shared_ptr<Window>;
}
namespace imp::gfx
{
	struct LoadedGLTF;
	class VulkanContext;
	class SwapChain;
	class DescriptorAllocatorGrowable;
	class ImmediateCommands;
	class FrameData;
	class Image;
	class ComputePipeline;
	class Buffer;
	class GUI;
	struct MetallicRoughness;
	struct Material;
	struct Mesh;
	struct GeoSurface;
	struct Vertex;
	struct DrawContext;
	class VKRenderer;
	struct Node;
	class IRenderable;
	struct MaterialInstance;
	struct MetallicRoughnessMaterialResources;
	struct MetallicRoughnessMaterialConstants;



	using UniqueSwapChain = std::unique_ptr< SwapChain>;
	using UniqueDescriptorAllocatorGrowable = std::unique_ptr< DescriptorAllocatorGrowable>;
	using UniqueImmediateCommands = std::unique_ptr< ImmediateCommands>;
	using UniqueFrameData = std::unique_ptr< FrameData>;
	using UniqueImage = std::unique_ptr< Image>;
	using SharedImage = std::shared_ptr< Image>;
	using UniqueComputePipeline = std::unique_ptr< ComputePipeline>;
	using SharedComputePipeline = std::shared_ptr< ComputePipeline>;
	using UniqueBuffer = std::unique_ptr< Buffer>;
	using SharedMaterial = std::shared_ptr< Material>;
	using UniqueMetallicRoughness = std::unique_ptr< MetallicRoughness>;
	using UniqueGUI = std::unique_ptr< GUI>;
	
	
}
