#pragma once
#include "InputCodes.h"

struct KeyEvent
{
	Input::Key key;
	int scancode;
	Input::Action action;
	Input::Mods mods;
};

struct MouseMoveEvent
{
	double x;
	double y;
};

struct MouseScrollEvent
{
	double x;
	double y;
};

struct MouseButtonEvent
{
	Input::MouseButton button;
	Input::Action action;
	Input::Mods mods;
};

struct MouseEnterEvent
{
	bool entered;
};

struct CursorHiddenEvent
{
	bool hidden;
};

struct WindowFocusEvent
{
	bool focused;
};
