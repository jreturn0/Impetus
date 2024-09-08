#pragma once
#include <cstdint>


enum class CVarFlags : uint32_t
{
	None = 1 << 0,
	NoEdit = 1 << 1,

};

template<typename T>
struct AutoCVar
{
protected:
	int index;
	using CVarType = T;
};

class ConsoleVariableSystem
{
public:
	
};
