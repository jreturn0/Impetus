#pragma once
#include "RenderObject.h"

namespace Imp::Render
{
	struct DrawContext
	{
		std::vector<RenderObject> opaque;
		std::vector<RenderObject> transparent;
	};
}
