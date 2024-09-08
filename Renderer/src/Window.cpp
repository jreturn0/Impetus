#include "pch.h"
#include "window/Window.h"

#include <glm/vec2.hpp>

#include "utils/VKCommon.hpp"

GLFWwindow* Imp::Window::CreateWindow(uint32_t width, uint32_t height, const char* title)
{

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	auto window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	//std::cin.get();
	return window;
}

void Imp::Window::getSize(uint32_t& _width, uint32_t& _height) const
{
	_width = width;
	_height = height;
}
glm::vec2 Imp::Window::getSize() const
{
	return glm::vec2(width, height);
}
Imp::Window::Window(uint32_t width, uint32_t height, const char* title) : glfw(CreateWindow(width, height, title)), width(width), height(height), title(title)
{}

::Imp::Window::~Window()
{
	glfwDestroyWindow(glfw);
	glfwTerminate();
}

void Imp::Window::setCursorMode(CursorMode mode)
{
	cursorMode = mode;
	switch (cursorMode) {
	case CursorMode::Normal:
		glfwSetInputMode(glfw, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case CursorMode::Hidden:
		glfwSetInputMode(glfw, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	case CursorMode::Disabled:
		glfwSetInputMode(glfw, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	case CursorMode::Captured:
		glfwSetInputMode(glfw, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	}
}


