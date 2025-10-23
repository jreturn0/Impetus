#pragma once
#include "Events/InputEvents.h"
#include "Events/SystemEvents.h"
#include "FSNavigator.h"
#include "InputCodes.h"
#include "UpdateType.h"
#include <entt/core/hashed_string.hpp>
#include <entt/core/type_traits.hpp>
#include <map>
#include <string>

using namespace entt::literals;


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




