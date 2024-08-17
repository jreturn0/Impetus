#pragma once
#include <string>
#include <vector>

#include "UpdateType.h"

struct SystemStatusEvent
{
	std::string name;
	bool status;
};

struct SystemsReorderedEvent
{

	std::vector<std::pair<std::string, bool>> variable, preFixed, fixed, postFixed;
};

struct SystemReorderEvent
{
	std::string name;
	Imp::UpdateType type;
	 size_t newIndex;
};

struct AddSystemEvent
{
	std::string name;
	Imp::UpdateType type;
};

struct RemoveSystemEvent
{
	std::string name;
};