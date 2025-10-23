#pragma once
#include "RenderObject.h"
#include "gpudata/GPULight.h"
namespace imp::gfx
{
    // Structure to hold opaque and transparent RenderObjects to be drawn
    struct DrawContext
    {
        std::vector<RenderObject> opaque{};
        std::vector<RenderObject> transparent{};
        std::vector<GPULight> lights{};
    };
}
