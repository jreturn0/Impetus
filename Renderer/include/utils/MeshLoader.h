#pragma once
#include <filesystem>
#include <optional>


#include "core/Vma.h"
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	struct LoadedGLTF;
	class VKRenderer;
	class CommandPool;
	class Device;

	class MeshLoader
	{
	private:


	public:
		//DISABLE_COPY_AND_MOVE(MeshLoader);

		static std::optional<std::shared_ptr<LoadedGLTF>> LoadGltf(std::string_view filePath, VKRenderer& renderer);
		//std::optional<std::vector<std::shared_ptr<struct Mesh>>> LoadGltfMeshes( std::filesystem::path filePath);
		
	};

	
}
