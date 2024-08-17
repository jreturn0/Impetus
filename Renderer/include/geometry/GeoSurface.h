#pragma once
#include <memory>

#include "Bounds.h"
#include "materials/MaterialPass.h"


namespace Imp::Render
{

	struct GeoSurface
	{
		uint32_t startIndex;
		uint32_t count;
		Bounds bounds;
		//std::string materialName;
		//MaterialPass passType;
		std::shared_ptr<struct Material> material;
	};
}
