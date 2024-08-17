#pragma once
#include <cstdint>
namespace Imp {
	enum  UpdateType : uint8_t
	{
		Variable,
		Fixed,
		PreFixed,
		PostFixed
	};
}
