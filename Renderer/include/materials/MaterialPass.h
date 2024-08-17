#pragma once
#include <cstdint>

namespace Imp::Render {
	enum class MaterialPass : uint8_t
	{
		MainColor,
		Transparent,
		Other
	};
}
