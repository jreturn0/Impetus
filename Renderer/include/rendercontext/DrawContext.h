#pragma once
#include "RenderObject.h"

namespace imp::gfx
{
	struct DrawContext
	{
		std::vector<RenderObject> opaque;
		std::vector<RenderObject> transparent;
	};
}
