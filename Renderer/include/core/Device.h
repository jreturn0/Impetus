#pragma once
#include <optional>

#include "QueueFamilyIndices.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class Instance;
	class VKWindow;


	class Device
	{
	private:
		vk::PhysicalDevice physical;
		QueueFamilyIndices indices;
		vk::UniqueDevice logical;
	public:
		explicit Device(const vk::Instance& instance, const vk::SurfaceKHR& surface);
		Device()=default;
		~Device();
		DISABLE_COPY_AND_MOVE(Device)
		operator const vk::PhysicalDevice& () const { return physical; }
		operator const vk::Device& () const { return logical.get(); }
		[[nodiscard]] const vk::PhysicalDevice& getPhysical() const { return physical; }
		[[nodiscard]] const QueueFamilyIndices& getIndices() const { return indices; }
		[[nodiscard]] const vk::Device& getLogical() const { return logical.get(); }

		
	};
	
	using UniqueDevice = std::unique_ptr<class Device>;
	UniqueDevice CreateUniqueDevice(const Instance& instance, const VKWindow& surface);
}
