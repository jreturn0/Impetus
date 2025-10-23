#include "CoreSystems/InputSystem.h"

#include <iostream>

#include "CtxRef.h"
#include "Debug.h"
#include "Renderer.h"
#include "Components/InputStateComponent.h"
#include "window/Window.h"

void imp::InputSystem::initialize(entt::registry& registry)
{
    m_keyStates = { 
        {Input::Key::W, Input::State::Released},
        {Input::Key::A, Input::State::Released},
        {Input::Key::S, Input::State::Released},
        {Input::Key::D, Input::State::Released},
        {Input::Key::LeftShift, Input::State::Released},
        {Input::Key::Space, Input::State::Released} 
    };

    m_mouseButtonStates = {
        {Input::MouseButton::Left, Input::State::Released},
        {Input::MouseButton::Right, Input::State::Released},
        {Input::MouseButton::Middle, Input::State::Released},
    };
    m_mouseX = m_mouseY = m_lastMouseX = m_lastMouseY = 0;


    auto& dispatcher = registry.ctx().get<CtxRef<entt::dispatcher>>().get();
    dispatcher.sink<KeyEvent>().connect<&InputSystem::OnKeyEvent>(this);
    dispatcher.sink<MouseButtonEvent>().connect<&InputSystem::OnMouseButtonEvent>(this);
    dispatcher.sink<MouseMoveEvent>().connect<&InputSystem::OnMouseMoveEvent>(this);
    dispatcher.sink<MouseScrollEvent>().connect<&InputSystem::OnMouseScrollEvent>(this);
    dispatcher.sink<WindowFocusEvent>().connect<&InputSystem::OnWindowFocusEvent>(this);
    dispatcher.sink<MouseEnterEvent>().connect<&InputSystem::OnMouseEnterEvent>(this);
    if (auto&& group = registry.group_if_exists<InputStateComponent>(); group.empty())
    {
        registry.emplace_or_replace<InputStateComponent>(registry.create(), m_keyStates, m_mouseButtonStates, m_mouseX, m_mouseY, m_lastMouseX, m_lastMouseY, m_scrollX, m_scrollY, m_mouseCaptured);
    }
}

void imp::InputSystem::OnMouseMoveEvent(const MouseMoveEvent mouseMoveEvent)
{
    m_lastMouseX = m_mouseX;
    m_lastMouseY = m_mouseY;
    m_mouseX = mouseMoveEvent.x;
    m_mouseY = mouseMoveEvent.y;
    m_mouseMoved = true;
}
void imp::InputSystem::OnMouseScrollEvent(const MouseScrollEvent mouseScrollEvent)
{
    m_scrollX = mouseScrollEvent.x;
    m_scrollY = mouseScrollEvent.y;
}

void imp::InputSystem::OnMouseEnterEvent(const MouseEnterEvent mouseEnterEvent)
{
    m_mouseEntered = mouseEnterEvent.entered;
}


void imp::InputSystem::OnWindowFocusEvent(const WindowFocusEvent windowFocusEvent)
{
    m_inputCaptured = windowFocusEvent.focused;
}



void imp::InputSystem::OnKeyEvent(KeyEvent keyEvent)
{
    if (keyEvent.action == Input::Action::Press) {
        m_keyStates[keyEvent.key] = (m_keyStates[keyEvent.key] == Input::State::Pressed || m_keyStates[keyEvent.key] == Input::State::JustPressed)
            ? Input::State::Pressed
            : Input::State::JustPressed;
        m_keyReleaseCheck.emplace(static_cast<uint32_t>(keyEvent.key));

    } else if (keyEvent.action == Input::Action::Release) {
        m_keyStates[keyEvent.key] = Input::State::JustReleased;
        m_keyReleaseCheck.emplace(static_cast<uint32_t>(keyEvent.key));
    } else if (keyEvent.action == Input::Action::Repeat) {
        m_keyStates[keyEvent.key] = Input::State::Pressed;
    }
}


void imp::InputSystem::OnMouseButtonEvent(MouseButtonEvent mouseButtonEvent)
{

    if (mouseButtonEvent.action == Input::Action::Press) {
        m_mouseButtonStates[mouseButtonEvent.button] = (m_mouseButtonStates[mouseButtonEvent.button] == Input::State::Pressed || m_mouseButtonStates[mouseButtonEvent.button] == Input::State::JustPressed)
            ? Input::State::Pressed
            : Input::State::JustPressed;
        m_buttonReleaseCheck.emplace(static_cast<uint32_t>(mouseButtonEvent.button));
    } else if (mouseButtonEvent.action == Input::Action::Release) {
        m_mouseButtonStates[mouseButtonEvent.button] = Input::State::JustReleased;
        m_buttonReleaseCheck.emplace(static_cast<uint32_t>(mouseButtonEvent.button));
    } else if (mouseButtonEvent.action == Input::Action::Repeat) {
        m_mouseButtonStates[mouseButtonEvent.button] = Input::State::Pressed;
    }
}



void imp::InputSystem::update(entt::registry& registry, const float deltaTime)
{

    auto group = registry.group<InputStateComponent>();
    if (group.empty()) {
        auto entity = registry.create();
        registry.emplace_or_replace<InputStateComponent>(entity, m_keyStates, m_mouseButtonStates, m_mouseX, m_mouseY, m_lastMouseX, m_lastMouseY, m_scrollX, m_scrollY, m_mouseCaptured);
        group = registry.group<InputStateComponent>();
    }

    group.each([this](auto entity, InputStateComponent& inputState) {
        if (!m_inputCaptured && !m_mouseEntered) {
            inputState.keyStates.clear();
            inputState.mouseButtonStates.clear();
            inputState.mouseX = inputState.mouseY = 0.0;
            inputState.lastMouseX = inputState.lastMouseY = 0.0;
            inputState.scrollX = inputState.scrollY = 0.0;
            return;
        }

        if (m_inputCaptured) {
            inputState.keyStates = m_keyStates;
        } else {
            inputState.keyStates.clear();
        }

        if (m_mouseEntered) {
            inputState.mouseButtonStates = m_mouseButtonStates;
            inputState.mouseX = m_mouseX;
            inputState.mouseY = m_mouseY;
            inputState.lastMouseX = m_lastMouseX;
            inputState.lastMouseY = m_lastMouseY;
            inputState.scrollX = m_scrollX;
            inputState.scrollY = m_scrollY;
        } else {
            inputState.mouseButtonStates.clear();
            inputState.mouseX = inputState.mouseY = 0.0;
            inputState.lastMouseX = inputState.lastMouseY = 0.0;
            inputState.scrollX = inputState.scrollY = 0.0;
        }
               });

    while (!m_keyReleaseCheck.empty()) {
        auto key = static_cast<Input::Key>(m_keyReleaseCheck.front());
        if (m_keyStates[key] == Input::State::JustReleased)
            m_keyStates[key] = Input::State::Released;
        if (m_keyStates[key] == Input::State::JustPressed)
            m_keyStates[key] = Input::State::Pressed;
        m_keyReleaseCheck.pop();
    }

    while (!m_buttonReleaseCheck.empty()) {
        auto button = static_cast<Input::MouseButton>(m_buttonReleaseCheck.front());
        if (m_mouseButtonStates[button] == Input::State::JustReleased)
            m_mouseButtonStates[button] = Input::State::Released;
        if (m_mouseButtonStates[button] == Input::State::JustPressed)
            m_mouseButtonStates[button] = Input::State::Pressed;
        m_buttonReleaseCheck.pop();
    }

    if (m_mouseMoved) {
        m_lastMouseX = m_mouseX;
        m_lastMouseY = m_mouseY;
        m_mouseMoved = false;
    }

    m_scrollX = m_scrollY = 0.0;

}

void imp::InputSystem::cleanup(entt::registry& registry)
{
    auto group = registry.group<InputStateComponent>();
    group.each([this](auto entity, InputStateComponent& inputState) {
        inputState.keyStates.clear();
        inputState.mouseButtonStates.clear();
        inputState.mouseX = inputState.mouseY = 0.0;
        inputState.lastMouseX = inputState.lastMouseY = 0.0;
        inputState.scrollX = inputState.scrollY = 0.0;
               });
    m_keyStates.clear();
    m_mouseButtonStates.clear();
    m_keyReleaseCheck = std::queue<uint32_t>();
    m_buttonReleaseCheck = std::queue<uint32_t>();

    Debug::Info("Input System Cleaned");
    Debug::Info("keyStates.size: {}, MouseButtonStates.size: {}", m_keyStates.size(), m_mouseButtonStates.size());
}
