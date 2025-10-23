#pragma once
#include <memory>

#include "Bounds.h"
namespace imp::gfx {
    // Structure representing geometry surface data
    struct GeoSurface
    {
        uint32_t startIndex{};
        uint32_t count{};
        Bounds bounds{};
        std::shared_ptr<struct Material> material{nullptr};
    };
}
