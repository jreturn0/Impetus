#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class Instance;
	using UniqueDebugMessenger = std::unique_ptr<class DebugMessenger>;

	class DebugMessenger
	{
	private:
		vk::DispatchLoaderDynamic dldi;
		vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> debugMessenger;
		static VKAPI_ATTR  VkBool32 VKAPI_CALL DebugMessageCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* /*pUserData*/);

		static inline vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> CreateDebugUtilsMessenger(
			const vk::Instance& instance, const vk::DispatchLoaderDynamic& dldi);
		DebugMessenger(const Instance& instance);


		friend UniqueDebugMessenger CreateUniqueDebugMessenger(const Instance& instance);

	public:
		DebugMessenger();
		~DebugMessenger();
		DISABLE_COPY_AND_MOVE(DebugMessenger);

	};

	UniqueDebugMessenger CreateUniqueDebugMessenger(const Instance& instance);
}
