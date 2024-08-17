#pragma once
#include "VKCommon.hpp"


namespace Imp::Render {
	class Device;
	class CommandPool;
	namespace vkutil {

		void CopyBuffer(const Device& device,
		                const vk::Queue& queue,
		                const CommandPool& commandPool,
		                const vk::Buffer& srcBuffer,
		                const vk::Buffer& dstBuffer,
		                vk::DeviceSize size, vk::DeviceSize srcOffset, vk::DeviceSize dstOffset);

		//function to copy staging src buffer to dst buffer
		//void StageCopyVertices( )
		//void CopyBuffer(const Imp::Render::Device& device,
//				const Imp::Render::CommandPool& pool,
//				const Imp::Render::Queue& queue,
//				const Imp::Render::Buffer& srcBuffer,
//				const Imp::Render::Buffer& dstBuffer,
//				vk::DeviceSize size)
//{
//
//}
//static Imp::Render::UniqueBuffer CreateVertBuffer(const Imp::Render::Device& device,
//												  const Imp::Render::CommandPool& pool,
//												  const Imp::Render::Queue& queue,
//												  const std::vector<Vertex>& vertices
//)
//{
//	vk::DeviceSize size = sizeof(vertices[0]) * vertices.size();
//	std::set<uint32_t> queueFamilyIndices = {
//	device.getIndices().graphicsFamily.value(),
//	device.getIndices().transferFamily.value()
//	};
//
//	// Create staging buffer with host visible memory
//	auto stagingBuffer = Imp::Render::CreateUniqueBuffer(device, size, (void*)&vertices[0],
//														 vk::BufferUsageFlagBits::eTransferSrc,
//														 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
//														 vk::SharingMode::eConcurrent, queueFamilyIndices);
//
//	// Create vertex buffer with device local memory
//	auto vertexBuffer = Imp::Render::CreateUniqueBuffer(device, size, nullptr,
//														vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
//														vk::MemoryPropertyFlagBits::eDeviceLocal,
//														vk::SharingMode::eConcurrent, queueFamilyIndices);
//
//	// Copy data from staging buffer to vertex buffer
//	CopyBuffer(device, pool, queue, *stagingBuffer, *vertexBuffer, size);
//
//	return vertexBuffer;
//}
//static Imp::Render::UniqueBuffer CreateIndexBuffer(const Imp::Render::Device& device,
//												   const Imp::Render::CommandPool& pool,
//												   const Imp::Render::Queue& queue,
//												   const  std::vector<uint16_t>& indices
//)
//{
//	vk::DeviceSize size = sizeof(indices[0]) * indices.size();
//	std::set<uint32_t> queueFamilyIndices = {
//	device.getIndices().graphicsFamily.value(),
//	device.getIndices().transferFamily.value()
//	};
//
//	// Create staging buffer with host visible memory
//	auto stagingBuffer = Imp::Render::CreateUniqueBuffer(device, size, (void*)&indices[0],
//														 vk::BufferUsageFlagBits::eTransferSrc,
//														 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
//														 vk::SharingMode::eConcurrent, queueFamilyIndices);
//
//	// Create vertex buffer with device local memory
//	auto vertexBuffer = Imp::Render::CreateUniqueBuffer(device, size, nullptr,
//														vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
//														vk::MemoryPropertyFlagBits::eDeviceLocal,
//														vk::SharingMode::eConcurrent, queueFamilyIndices);
//
//	// Copy data from staging buffer to vertex buffer
//	CopyBuffer(device, pool, queue, *stagingBuffer, *vertexBuffer, size);
//
//	return vertexBuffer;
//}

	}
}