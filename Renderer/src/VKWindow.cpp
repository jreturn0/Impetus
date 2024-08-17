#include "window/VKWindow.h"
#include <GLFW/glfw3.h>
vk::UniqueSurfaceKHR Imp::Render::VKWindow::createSurface(const vk::Instance& instance) const
{
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(instance, glfw, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}

	return vk::UniqueSurfaceKHR(surface, instance);
}

Imp::Render::VKWindow::VKWindow(uint32_t _width, uint32_t _height, const char* _title, const vk::Instance& instance):
	Window(_width,_height,_title), surface(std::move(createSurface(instance)))
{}

Imp::Render::VKWindow::VKWindow(uint32_t _width, uint32_t _height, const char* _title):Window(_width,_height,_title)
{
	//std::cin.get();
}

void Imp::Render::VKWindow::setSurface(const vk::Instance& instance)
{
	surface = std::move(createSurface(instance));
}


