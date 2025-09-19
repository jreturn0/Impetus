//#include "core/Instance.h"
//
//#include "window/VKWindow.h"
//VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
//vk::UniqueInstance Imp::Render::Instance::CreateInstance(const char* title)
//{
//	auto version = VK_MAKE_API_VERSION(0, 1, 0, 0);
//	vk::ApplicationInfo appInfo(
//		title,
//		version,
//		"Impetus",
//		version,
//		VK_API_VERSION_1_4
//	);
//
//	std::vector<const char*> extensions = vkutil::GetRequiredExtensions();
//
//	if (!vkutil::CheckExtensionsAvailability(extensions)) {
//		throw std::runtime_error("Required extensions are not available");
//	}
//	if (vkutil::ENABLE_VALIDATION_LAYERS && !vkutil::CheckValidationLayerSupport()) {
//		throw std::runtime_error("Validation layers are not available");
//	}
//
//	vk::InstanceCreateInfo instanceCreateInfo(
//		{},
//		&appInfo,
//		vkutil::ENABLE_VALIDATION_LAYERS ? static_cast<uint32_t>(vkutil::VALIDATION_LAYERS.size()) : 0,
//		vkutil::ENABLE_VALIDATION_LAYERS ? vkutil::VALIDATION_LAYERS.data() : nullptr,
//		static_cast<uint32_t>(extensions.size()),
//		extensions.data()
//	);
//
//	auto instance = vk::createInstanceUnique(instanceCreateInfo);
//
//
//	VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance);
//
//	return instance;
//}
//
//
//Imp::Render::Instance::Instance(const char* title) : instance(CreateInstance(title))
//{
//}
//
//Imp::Render::Instance::~Instance()
//{
//}
//
//Imp::Render::UniqueInstance Imp::Render::CreateUniqueInstance(const char* title, VKWindow& window)
//{
//	auto instance = std::unique_ptr<Instance>(new Instance(title));
//	window.setSurface(*instance);
//	//VULKAN_HPP_DEFAULT_DISPATCHER.init((*instance).getInstance());
//	return std::move(instance);
//}
