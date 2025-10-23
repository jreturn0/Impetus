#pragma once
#include <cstdint>
namespace imp {
	enum  UpdateType : uint8_t
	{
		Variable,
		Fixed,
		PreFixed,
		PostFixed
	};
}
