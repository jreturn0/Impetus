
#include "window/Window.h"
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <string_view>
#include "ConfigSystem.h"
namespace {

    utl::ConfigValueRef cfg_windowWidth("window.width", 1280);
    utl::ConfigValueRef cfg_windowHeight("window.height", 720);
    utl::ConfigValueRef<std::string> s_windowTitle("window.title", "Impetus");
    utl::ConfigValueRef<bool> s_windowResizeable("window.resizeable", true);
}

GLFWwindow* imp::Window::CreateWindow()
{
    auto&& globalCfg = utl::ConfigSystem::instance().getGlobalConfigFile();



    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, s_windowResizeable.getCopy());



    // These values are optional, only set them if they exist in the config
    glfwWindowHint(GLFW_DECORATED, globalCfg.getValueCopyOrDefault("window.decorated", true));
    glfwWindowHint(GLFW_MAXIMIZED, globalCfg.getValueCopyOrDefault("window.maximized", false));
    glfwWindowHint(GLFW_CENTER_CURSOR, globalCfg.getValueCopyOrDefault("window.centerCursor", false));
    glfwWindowHint(GLFW_FLOATING, globalCfg.getValueCopyOrDefault("window.floating", false));


    auto window = glfwCreateWindow(static_cast<int32_t>(cfg_windowWidth.getCopy()), static_cast<int32_t>(cfg_windowHeight.getCopy()), s_windowTitle.getCopy().c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
    }


    //std::cin.get();
    return window;
}


imp::Window::Window(std::string_view title) : m_glfw(CreateWindow()), m_width(cfg_windowWidth.getCopy()), m_height(cfg_windowHeight.getCopy()), m_title(title)
{
}

::imp::Window::~Window()
{
    glfwDestroyWindow(m_glfw);
    glfwTerminate();
}

void imp::Window::setCursorMode(CursorMode mode)
{
    m_cursorMode = mode;
    switch (m_cursorMode) {
    case CursorMode::Normal:
        glfwSetInputMode(m_glfw, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case CursorMode::Hidden:
        glfwSetInputMode(m_glfw, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        break;
    case CursorMode::Disabled:
        glfwSetInputMode(m_glfw, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    case CursorMode::Captured:
        glfwSetInputMode(m_glfw, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    }
}


