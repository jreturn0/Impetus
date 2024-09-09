#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render
{
	class Device;
	using UniqueResourceManager = std::unique_ptr<class ResourceManager>;

	class ResourceManager
	{
	private:
		class VKRenderer* renderer;





	public:
		ResourceManager(const VKRenderer& device);
		ResourceManager();
		~ResourceManager();
		DISABLE_COPY_AND_MOVE(ResourceManager);
		
	};


}
