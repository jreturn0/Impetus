//#pragma once
//#include <optional>
//
//#include "QueueFamilyIndices.h"
//#include "utils/VKCommon.hpp"
//#include "utils/QuickMacros.h"
//
//namespace Imp::Render
//{
//	class Instance;
//	class VKWindow;
//
//	namespace vkutil
//	{
//		
//	}
//	using UniqueDevice = std::unique_ptr<class Device>;
//	class Device
//	{
//	private:
//		vk::PhysicalDevice physical;
//		QueueFamilyIndices indices;
//		vk::UniqueDevice logical;
//		static bool IsDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
//		static bool CheckDeviceExtensionSupport(const vk::PhysicalDevice& device);
//		static vk::PhysicalDevice PickPhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface);
//		static vk::UniqueDevice CreateDevice(const vk::PhysicalDevice& physicalDevice,
//									  const QueueFamilyIndices& indices);
//
//		friend UniqueDevice CreateUniqueDevice(const Instance& instance, const VKWindow& surface);
//		Device(const vk::Instance& instance, const vk::SurfaceKHR& surface);
//	public:
//		Device()=default;
//		~Device();
//		DISABLE_COPY_AND_MOVE(Device);
//		operator const vk::PhysicalDevice& () const { return physical; }
//		operator const vk::Device& () const { return logical.get(); }
//		const vk::PhysicalDevice& getPhysical() const { return physical; }
//		const QueueFamilyIndices& getIndices() const { return indices; }
//		const vk::Device& getLogical() const { return logical.get(); }
//
//
//		
//	};
//	
//	[[deprecated]]
//	UniqueDevice CreateUniqueDevice(const Instance& instance, const VKWindow& surface);
//}
