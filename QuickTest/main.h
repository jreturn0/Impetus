#pragma once
#include "BitFlags.h"
enum class NonBitmaskFlags {
	none = 0,
	flag1 = 1,
	flag2 = 2,
	flag3 = 3,
	flag4 = 4,
};

enum class TestFlagBits : uint16_t {
	none = 0,
	flag1 = 1 << 0,
	flag2 = 1 << 1,
	flag3 = 1 << 2,
	flag4 = 1 << 3,

};

//using TestFlags = utl::BitFlags<TestFlagBits>;
//template <>
//struct utl::FlagTraits<TestFlagBits> {
//	static constexpr bool isBitmask = true;
//	static constexpr TestFlags allFlags = TestFlagBits::flag1 | TestFlagBits::flag2 | TestFlagBits::flag3 | TestFlagBits::flag4;
//}; 


//template<>
//struct utl::FlagTraits<TestFlagBits>
//{
//	static constexpr bool isBitmask = true;
//	//inline static constexpr TestFlagsMask allFlags = TestFlagBits::flag1 | TestFlagBits::flag2 | TestFlagBits::flag3 | TestFlagBits::flag4;
//};
//
