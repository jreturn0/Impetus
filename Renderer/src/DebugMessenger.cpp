//#include "core/DebugMessenger.h"
//#include <iostream>
//#include <sstream>
//#include "core/Instance.h"
//#include <Debug.h>
//
//#include "AnsiCodes.h"
//
//VKAPI_ATTR VkBool32 VKAPI_CALL Imp::Render::DebugMessenger::DebugMessageCallback(
//    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
//    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//       void* pUserData)
//   
//{
//	std::ostringstream message;
//
//	message << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes))
//		<< ": " << "\n"
//		<< "\tmessageIDName   = <" << pCallbackData->pMessageIdName << ">\n"
//		<< "\tmessageIdNumber = " << pCallbackData->messageIdNumber << "\n"
//		<< "\tmessage         = <" << pCallbackData->pMessage << ">\n";
//
//	if (pCallbackData->queueLabelCount > 0) {
//		message << "\tQueue Labels:\n";
//		for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
//			message << "\t\tlabelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
//		}
//	}
//
//	if (pCallbackData->cmdBufLabelCount > 0) {
//		message << "\tCommandBuffer Labels:\n";
//		for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; ++i) {
//			message << "\t\tlabelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
//		}
//	}
//
//	if (pCallbackData->objectCount > 0) {
//		message << "\tObjects:\n";
//		for (uint32_t i = 0; i < pCallbackData->objectCount; ++i) {
//			message << "\t\tObject " << i << "\n"
//				<< "\t\t\tobjectType   = " << vk::to_string(
//					static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) << "\n"
//				<< "\t\t\tobjectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
//			if (pCallbackData->pObjects[i].pObjectName) {
//				message << "\t\t\tobjectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
//			}
//		}
//	}
//
//
//
//
//	switch (messageSeverity) {
//	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
//		Debug::Info("{}",message.str());
//		break;
//	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
//		Debug::Info("{}", message.str());
//		break;
//	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
//		Debug::Warning("{}", message.str());
//		break;
//	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
//		 Debug::Error("{}", message.str());
//		break;
//	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
//		break;
//	}
//	//std::cout << message.str() << std::endl;
//
//	return VK_FALSE;
//}
//
//vk::UniqueDebugUtilsMessengerEXT  Imp::Render::DebugMessenger::
//CreateDebugUtilsMessenger(const vk::Instance& instance)
//{
//	auto severityFlags = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
//		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
//		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
//		vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
//	auto messageTypeFlags = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
//		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
//		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
//
//	vk::DebugUtilsMessengerCreateInfoEXT createInfo(
//		vk::DebugUtilsMessengerCreateFlagsEXT{},
//		severityFlags,
//		messageTypeFlags,
//		reinterpret_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(&Imp::Render::DebugMessenger::DebugMessageCallback),
//		nullptr
//	);
//	return instance.createDebugUtilsMessengerEXTUnique(createInfo);
//}
//
//Imp::Render::DebugMessenger::DebugMessenger(const Instance& instance) :
//debugMessenger(CreateDebugUtilsMessenger(instance))
//{
//
//}
//
//Imp::Render::DebugMessenger::DebugMessenger()
//{
//}
//
//Imp::Render::DebugMessenger::~DebugMessenger()
//{
//}
//
//Imp::Render::UniqueDebugMessenger Imp::Render::CreateUniqueDebugMessenger(const Instance& instance)
//{
//	return std::unique_ptr<DebugMessenger>(new DebugMessenger(instance));
//}
