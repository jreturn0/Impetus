#pragma once
//#include "utils/VKCommon.hpp"
#include <memory>
#include "utils/VKCommon.hpp"
#include "utils/QuickMacros.h"

namespace Imp::Render {
	using UniqueInstance = std::unique_ptr<class Instance>;
	class Instance
	{
	private:
		vk::UniqueInstance instance;
		static vk::UniqueInstance CreateInstance(const char* title);
		explicit Instance(const char* title);
		friend UniqueInstance CreateUniqueInstance(const char* title);

	public:
		Instance() = default;
		~Instance();
		DISABLE_COPY_AND_MOVE(Instance);
		operator const vk::Instance& ()const { return *instance; }
		operator vk::Instance& () { return *instance; }
		const vk::Instance& getInstance()const { return *instance; };
		

	};
	inline UniqueInstance CreateUniqueInstance(const char* title)
	{
		return std::unique_ptr<Instance>(new Instance(title));
	}


}
