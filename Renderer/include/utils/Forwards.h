#pragma once
#include <memory>

class ComputeEffect;
namespace Imp::Render
{
	class Instance;
	class VKWindow;
	class DebugMessenger;
	class Device;
	class SwapChain;
	class Queue;
	class Vma;
	class DescriptorAllocatorGrowable;
	class CommandPool;
	class CommandBuffer;
	class ImmediateCommands;
	class FrameData;
	class Image;
	class ComputePipeline;
	class GraphicsPipeline;
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


	using UniqueInstance = std::unique_ptr< Instance>;
	using SharedWindow = std::shared_ptr< VKWindow>;
	using UniqueDebugMessenger = std::unique_ptr< DebugMessenger>;
	using UniqueDevice = std::unique_ptr< Device>;
	using UniqueSwapChain = std::unique_ptr< SwapChain>;
	using UniqueQueue = std::unique_ptr< Queue>;
	using UniqueVmaAllocator = std::unique_ptr< Vma>;
	using UniqueDescriptorAllocatorGrowable = std::unique_ptr< DescriptorAllocatorGrowable>;
	using UniqueCommandPool = std::unique_ptr< CommandPool>;
	using UniqueCommandBuffer = std::unique_ptr< CommandBuffer>;
	using UniqueImmediateCommands = std::unique_ptr< ImmediateCommands>;
	using UniqueFrameData = std::unique_ptr< FrameData>;
	using UniqueImage = std::unique_ptr< Image>;
	using SharedImage = std::shared_ptr< Image>;
	using UniqueComputePipeline = std::unique_ptr< ComputePipeline>;
	using SharedComputePipeline = std::shared_ptr< ComputePipeline>;
	using UniqueGraphicsPipeline = std::unique_ptr< GraphicsPipeline>;
	using UniqueBuffer = std::unique_ptr< Buffer>;
	using SharedMaterial = std::shared_ptr< Material>;
	using UniqueMetallicRoughness = std::unique_ptr< MetallicRoughness>;
	using UniqueGUI = std::unique_ptr< GUI>;
	
	
}
