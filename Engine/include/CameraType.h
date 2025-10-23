#pragma once
#include <string_view>
namespace imp {
	enum class CameraType
	{
		Orthographic,
		Perspective
	};

	constexpr std::string_view ToString(const CameraType type)
	{
		switch (type) {
		case CameraType::Orthographic:
			return "Orthographic";
		case CameraType::Perspective:
			return "Perspective";
		default:
			return "Unknown";
		}
	}
}