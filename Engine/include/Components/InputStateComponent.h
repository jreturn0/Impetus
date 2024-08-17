#pragma once
#include <unordered_map>
#include <glm/vec2.hpp>

#include "InputCodes.h"
namespace Imp
{

	struct InputStateComponent
	{
		std::unordered_map<Input::Key, Input::State> keyStates;
		std::unordered_map<Input::MouseButton, Input::State> mouseButtonStates;
		double mouseX{ 0. }, mouseY{ 0. }, lastMouseX{ 0. }, lastMouseY{ 0. };
		double scrollX{ 0. }, scrollY{ 0. };
		bool cursorHidden= false;
		glm::vec2 getMousePosition() const
		{
			return glm::vec2(mouseX, mouseY);
		}
		glm::vec2 getMouseDelta() const
		{
			return glm::vec2( lastMouseX- mouseX, mouseY - lastMouseY);
		}
	};


}
