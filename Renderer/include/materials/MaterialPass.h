#pragma once
#include <cstdint>

namespace imp::gfx {
	// Enum for material render pass types
	enum class MaterialPass : uint8_t
	{
		Opaque,
		Transparent,
		Other
	};
}
