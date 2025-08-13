#pragma once
#include <memory>

#include "Bounds.h"
namespace Imp::Render {

	struct GeoSurface
	{
		uint32_t startIndex;
		uint32_t count;
		Bounds bounds;
		std::shared_ptr<struct Material> material;
	};
}
