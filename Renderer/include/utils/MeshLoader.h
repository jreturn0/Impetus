#pragma once
#include <optional>

#include "utils/Forwards.h"
#include <memory>
#include <string_view>

namespace imp::gfx
{
    class MeshLoader
    {
    public:
        static std::optional<std::shared_ptr<LoadedGLTF>> LoadGltf(std::string_view filePath, const VulkanContext& context, ResourceCache& cache, MetallicRoughness& metallicRoughness);
    };

    
}
