#pragma once
#include "utils/QuickMacros.h"
#include "utils/VKCommon.hpp"
#include <filesystem>
#include <optional>

namespace imp::gfx
{
    struct LoadedGLTF;
    class VulkanContext;
    class ResourceCache;
    class MetallicRoughness;
    class ImmediateCommands;
    class Device;
    
    // Utility class for loading 3D mesh data from GLTF files
    class MeshLoader
    {
    public:

        // Methods

        // Load a GLTF file and return a LoadedGLTF object if successful
        static std::optional<std::shared_ptr<LoadedGLTF>> LoadGltf(std::string_view filePath, const VulkanContext& context, ResourceCache& cache, MetallicRoughness& metallicRoughness);
    };

    
}
