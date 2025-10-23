#pragma once
#include <queue>
#include <thread>
#include <GLFW/glfw3.h>

#include "events/BasicEvents.h"
#include "System.h"



namespace imp {
    class InputSystem final : public System
    {
    public:
        void initialize(entt::registry& registry) override;
        void update(entt::registry& registry, const float deltaTime) override;
        void cleanup(entt::registry& registry) override;
    private:
        std::unordered_map<Input::Key, Input::State> m_keyStates{};
        std::unordered_map<Input::MouseButton, Input::State> m_mouseButtonStates{};
        double m_mouseX{ 0. }, m_mouseY{ 0. }, m_lastMouseX{ 0. }, m_lastMouseY{ 0. };
        double m_scrollX{ 0. }, m_scrollY{ 0. };
        bool m_mouseMoved{ false };
        bool m_keyPressed{ false };
        bool m_mouseButtonPressed{ false };
        bool m_inputCaptured{ true };
        bool m_mouseEntered{ true };
        bool m_mouseCaptured{ false };
        std::queue<uint32_t> m_keyReleaseCheck{};
        std::queue<uint32_t> m_buttonReleaseCheck{};

        void OnKeyEvent(KeyEvent keyEvent);
        void OnMouseMoveEvent(MouseMoveEvent mouseMoveEvent);
        void OnMouseButtonEvent(MouseButtonEvent mouseButtonEvent);
        void OnMouseScrollEvent(MouseScrollEvent mouseScrollEvent);
        void OnMouseEnterEvent(MouseEnterEvent mouseEnterEvent);
        void OnWindowFocusEvent(WindowFocusEvent windowFocusEvent);
    };
}