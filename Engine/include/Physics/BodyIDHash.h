#pragma once
#include <functional>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace imp::phys
{
	//JPH::BodyID custom hash function
	struct BodyIDHash
	{
		std::size_t operator()(const JPH::BodyID& id) const
		{
			return std::hash<uint64_t>()(id.GetIndexAndSequenceNumber());
		}
	};
}
