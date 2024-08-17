#pragma once
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render {
	class Device;
	using UniqueCommandPool = std::unique_ptr<class CommandPool>;

	class CommandPool
	{
	private:
		uint32_t familyIndex;
		vk::UniqueCommandPool pool;
		CommandPool(const Device& device, uint32_t familyIndex);


		friend UniqueCommandPool CreateUniqueCommandPool(const Device& device,uint32_t familyIndex);

	public:
		const vk::CommandPool& getPool()const { return *pool; }
		operator vk::CommandPool& () { return*pool; }
		CommandPool();
		~CommandPool();
		DISABLE_COPY_AND_MOVE(CommandPool);

	};

	UniqueCommandPool CreateUniqueCommandPool(const Device& device, uint32_t familyIndex);
}
