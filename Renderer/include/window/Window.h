#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>
#include <utility>
#include "utils/QuickMacros.h"
#include <string>
#include <functional>

struct GLFWwindow;
namespace imp {
    // Simple window wrapper around GLFW
    // TODO: window events should come from here
    class Window
    {
        using ResizeCallback = std::function<void(int, int)>;
    public:




        enum class CursorMode
        {
            Normal = 0x00034001,
            Hidden = 0x00034002,
            Disabled = 0x00034003,
            Captured = 0x00034004
        };

        // Default 1280x720
        // Edit inside of globalcfg.ini
        Window(std::string_view title);
        ~Window();
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;;





        void onResize() {
            int width = 0, height = 0;
            glfwGetWindowSize(m_glfw, &width, &height);
            while (width == 0 || height == 0) {
                glfwGetWindowSize(m_glfw, &width, &height);
                glfwWaitEvents();
            }
            m_width = width;
            m_height = height;
        }



        inline GLFWwindow* getGLFW() const { return m_glfw; }
        inline CursorMode getCursorMode() const { return cursorMode; }
        void setCursorMode(CursorMode mode);



        // GLFWwindow specific function wrappers


        bool getShouldClose() const { return glfwWindowShouldClose(m_glfw); }
        void setShouldClose(bool close) { glfwSetWindowShouldClose(m_glfw, close); }

        std::string_view getTitle() const { return m_title; }
        void setTitle(std::string_view title) { glfwSetWindowTitle(m_glfw, (m_title = title).c_str()); }

        void setPosition(int32_t x, int32_t y) { glfwSetWindowPos(m_glfw, x, y); }
        std::pair<int32_t, int32_t> getPosition() const {
            int32_t x, y;
            glfwGetWindowPos(m_glfw, &x, &y);
            return { x, y };
        }

        std::pair<int32_t, int32_t> getSize() const {
            int32_t width, height;
            glfwGetWindowSize(m_glfw, &width, &height);
            return { width, height };
        }
        std::pair<uint32_t, uint32_t> getExtent() const {
            return { static_cast<float>(m_width), static_cast<float>(m_height) };
        }
        inline uint32_t getWidth() const { return static_cast<uint32_t>(m_width); }
        inline uint32_t getHeight() const { return static_cast<uint32_t>(m_height); }

        glm::vec2 getSizeVec() const {
            return { static_cast<float>(m_width), static_cast<float>(m_height) };
        }

        void setSize(int32_t width, int32_t height) {
            m_width = width;
            m_height = height;
            glfwSetWindowSize(m_glfw, width, height);
        }

        











        void maximize() { glfwMaximizeWindow(m_glfw); }
        void minimize() { glfwIconifyWindow(m_glfw); }
        void restore() { glfwRestoreWindow(m_glfw); }
        


        void addResizeCallback(const ResizeCallback& callback) {
            m_resizeCallbacks.push_back(callback);
        }



    private:

        void onResize(GLFWwindow* window, int width, int height) {
            m_width = width;
            m_height = height;
        }


        std::vector<ResizeCallback> m_resizeCallbacks;

        GLFWwindow* m_glfw;
        int32_t m_width, m_height;
        std::string m_title;
        CursorMode cursorMode = CursorMode::Normal;

        static GLFWwindow* CreateWindow();
    };
}
