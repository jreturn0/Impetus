#pragma once
#include <map>
#include <entt/core/type_traits.hpp>
#include <entt/core/hashed_string.hpp>

#include "InputCodes.h"
#include <string>

#include "FSNavigator.h"
#include "UpdateType.h"
#include "Events/InputEvents.h"
#include "Events/SystemEvents.h"

using namespace entt::literals;

//shutdown and close engine
using ShutdownEvent = entt::tag<"Close"_hs>;
using ToggleGuiEvent = entt::tag<"ToggleGui"_hs>;

using QuickSaveSceneEvent = entt::tag<"QuickSaveScene"_hs>;

struct SaveSceneAsEvent
{
	FSNavigator path;
	std::string filename;
};

struct LoadSceneEvent
{
	FSNavigator path;
};




