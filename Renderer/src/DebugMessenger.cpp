#include "core/DebugMessenger.h"
#include <iostream>
#include <sstream>
#include "core/Instance.h"
#include <Debug.h>

#include "AnsiCodes.h"

VkBool32 Imp::Render::DebugMessenger::DebugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                           VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
	std::ostringstream message;

	message << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes))
		<< ": " << "\n"
		<< "\tmessageIDName   = <" << pCallbackData->pMessageIdName << ">\n"
		<< "\tmessageIdNumber = " << pCallbackData->messageIdNumber << "\n"
		<< "\tmessage         = <" << pCallbackData->pMessage << ">\n";

	if (pCallbackData->queueLabelCount > 0) {
		message << "\tQueue Labels:\n";
		for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
			message << "\t\tlabelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
		}
	}

	if (pCallbackData->cmdBufLabelCount > 0) {
		message << "\tCommandBuffer Labels:\n";
		for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; ++i) {
			message << "\t\tlabelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
		}
	}

	if (pCallbackData->objectCount > 0) {
		message << "\tObjects:\n";
		for (uint32_t i = 0; i < pCallbackData->objectCount; ++i) {
			message << "\t\tObject " << i << "\n"
				<< "\t\t\tobjectType   = " << vk::to_string(
					static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) << "\n"
				<< "\t\t\tobjectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
			if (pCallbackData->pObjects[i].pObjectName) {
				message << "\t\t\tobjectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
			}
		}
	}




	switch (messageSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		Debug::Info("{}",message.str());
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		Debug::Info("{}", message.str());
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		Debug::Warning("{}", message.str());
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		 Debug::Error("{}", message.str());
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
		break;
	}
	//std::cout << message.str() << std::endl;

	return VK_FALSE;
}

vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> Imp::Render::DebugMessenger::
CreateDebugUtilsMessenger(const vk::Instance& instance, const vk::DispatchLoaderDynamic& dldi)
{
	vk::DebugUtilsMessengerCreateInfoEXT createInfo(
		{},
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		DebugMessenger::DebugMessageCallback
	);
	return instance.createDebugUtilsMessengerEXTUnique(createInfo, nullptr, dldi);
}

Imp::Render::DebugMessenger::DebugMessenger(const Instance& instance) : dldi(instance.getInstance(), vkGetInstanceProcAddr),
debugMessenger(CreateDebugUtilsMessenger(instance,dldi))
{

}

Imp::Render::DebugMessenger::DebugMessenger()
{
}

Imp::Render::DebugMessenger::~DebugMessenger()
{
}

Imp::Render::UniqueDebugMessenger Imp::Render::CreateUniqueDebugMessenger(const Instance& instance)
{
	return std::unique_ptr<DebugMessenger>(new DebugMessenger(instance));
}
