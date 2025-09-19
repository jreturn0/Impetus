#pragma once
#include <cstdint>

namespace imp::gfx {
	enum class MaterialPass : uint8_t
	{
		MainColor,
		Transparent,
		Other
	};
}
