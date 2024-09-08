#include "pch.h"
#include "core/Instance.h"

#include "window/VKWindow.h"

vk::UniqueInstance Imp::Render::Instance::CreateInstance(const char* title)
{
	auto version = VK_MAKE_API_VERSION(0, 1, 0, 0);
	vk::ApplicationInfo appInfo(
		title,
		version,
		"Impetus",
		version,
		VK_API_VERSION_1_3
	);

	std::vector<const char*> extensions = vkutil::GetRequiredExtensions();

	if (!vkutil::CheckExtensionsAvailability(extensions)) {
		Debug::Throw("Required extensions are not available");
	}


	if constexpr (vkutil::ENABLE_VALIDATION_LAYERS) {
		if (!vkutil::CheckValidationLayerSupport()) {
			Debug::Throw("Validation layers are not available");
		}
	}
	constexpr auto layerCount = vkutil::ENABLE_VALIDATION_LAYERS ? static_cast<uint32_t>(vkutil::VALIDATION_LAYERS.size()) : 0;
	constexpr auto layers = vkutil::ENABLE_VALIDATION_LAYERS ? vkutil::VALIDATION_LAYERS.data() : nullptr;
	return vk::createInstanceUnique({
		{},
		& appInfo,
		layerCount,
		layers,
		static_cast<uint32_t>(extensions.size()),
		extensions.data()
	});
}

Imp::Render::Instance::Instance(const char* title) : instance(CreateInstance(title))
{}

Imp::Render::Instance::~Instance()
= default;


