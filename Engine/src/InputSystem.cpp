#include "CoreSystems/InputSystem.h"

#include <iostream>

#include "CtxRef.h"
#include "Debug.h"
#include "Renderer.h"
#include "Components/InputStateComponent.h"
#include "window/Window.h"

void Imp::InputSystem::initialize(entt::registry& registry)
{
	if (initalized) {
		//Debug::Out("Input System (id: {}) was already Initialized", id);
		return;
	}
	keyStates = { {Input::Key::W, Input::State::Released},
		{Input::Key::A, Input::State::Released},
		{Input::Key::S, Input::State::Released},
		{Input::Key::D, Input::State::Released},
		{Input::Key::LeftShift, Input::State::Released},
		{Input::Key::Space, Input::State::Released} };

	mouseButtonStates = {
		{Input::MouseButton::Left, Input::State::Released},
		{Input::MouseButton::Right, Input::State::Released},
		{Input::MouseButton::Middle, Input::State::Released},
	};
	mouseX = mouseY = lastMouseX = lastMouseY = 0;


	auto& dispatcher = registry.ctx().get<CtxRef<entt::dispatcher>>().get();
	dispatcher.sink<KeyEvent>().connect<&InputSystem::OnKeyEvent>(this);
	dispatcher.sink<MouseButtonEvent>().connect<&InputSystem::OnMouseButtonEvent>(this);
	dispatcher.sink<MouseMoveEvent>().connect<&InputSystem::OnMouseMoveEvent>(this);
	dispatcher.sink<MouseScrollEvent>().connect<&InputSystem::OnMouseScrollEvent>(this);
	dispatcher.sink<WindowFocusEvent>().connect<&InputSystem::OnWindowFocusEvent>(this);
	dispatcher.sink<MouseEnterEvent>().connect<&InputSystem::OnMouseEnterEvent>(this);
	if (auto&& group = registry.group_if_exists<InputStateComponent>(); group.empty())
	{
		registry.emplace_or_replace<InputStateComponent>(registry.create(), keyStates, mouseButtonStates, mouseX, mouseY, lastMouseX, lastMouseY, scrollX, scrollY, mouseCaptured);
	}
	//Debug::Error("Input System (id: {}) Initialized", id);
	//Debug::Info("Input System Initialized");
	//Debug::Info("keyStates.size: {}, MouseButtonStates.size: {}", keyStates.size(), mouseButtonStates.size());
	initalized = true;


}

void Imp::InputSystem::OnMouseMoveEvent(const MouseMoveEvent mouseMoveEvent)
{
	lastMouseX = mouseX;
	lastMouseY = mouseY;
	mouseX = mouseMoveEvent.x;
	mouseY = mouseMoveEvent.y;
	mouseMoved = true;
}
void Imp::InputSystem::OnMouseScrollEvent(const MouseScrollEvent mouseScrollEvent)
{
	scrollX = mouseScrollEvent.x;
	scrollY = mouseScrollEvent.y;
}

void Imp::InputSystem::OnMouseEnterEvent(const MouseEnterEvent mouseEnterEvent)
{
	//Imp::Debug::Info("Mouse Enter Event: {}", mouseEnterEvent.entered);
	mouseEntered = mouseEnterEvent.entered;
}


void Imp::InputSystem::OnWindowFocusEvent(const WindowFocusEvent windowFocusEvent)
{
	//Imp::Debug::Info("Window Focus Event: {}", windowFocusEvent.focused);

	inputCaptured = windowFocusEvent.focused;
}



void Imp::InputSystem::OnKeyEvent(KeyEvent keyEvent)
{

	//Debug::Out("id problem: {}", id);

	if (keyEvent.action == Input::Action::Press) {
		keyStates[keyEvent.key] = (keyStates[keyEvent.key] == Input::State::Pressed || keyStates[keyEvent.key] == Input::State::JustPressed)
			? Input::State::Pressed
			: Input::State::JustPressed;
		keyReleaseCheck.emplace(static_cast<uint32_t>(keyEvent.key));

	} else if (keyEvent.action == Input::Action::Release) {
		keyStates[keyEvent.key] = Input::State::JustReleased;
		keyReleaseCheck.emplace(static_cast<uint32_t>(keyEvent.key));
	} else if (keyEvent.action == Input::Action::Repeat) {
		keyStates[keyEvent.key] = Input::State::Pressed;
	}
}


void Imp::InputSystem::OnMouseButtonEvent(MouseButtonEvent mouseButtonEvent)
{

	if (mouseButtonEvent.action == Input::Action::Press) {
		mouseButtonStates[mouseButtonEvent.button] = (mouseButtonStates[mouseButtonEvent.button] == Input::State::Pressed || mouseButtonStates[mouseButtonEvent.button] == Input::State::JustPressed)
			? Input::State::Pressed
			: Input::State::JustPressed;
		buttonReleaseCheck.emplace(static_cast<uint32_t>(mouseButtonEvent.button));
	} else if (mouseButtonEvent.action == Input::Action::Release) {
		mouseButtonStates[mouseButtonEvent.button] = Input::State::JustReleased;
		buttonReleaseCheck.emplace(static_cast<uint32_t>(mouseButtonEvent.button));
	} else if (mouseButtonEvent.action == Input::Action::Repeat) {
		mouseButtonStates[mouseButtonEvent.button] = Input::State::Pressed;
	}
}



void Imp::InputSystem::update(entt::registry& registry, const float deltaTime)
{

	auto group = registry.group<InputStateComponent>();
	group.each([this](auto entity, InputStateComponent& inputState) {
		if (!inputCaptured && !mouseEntered) {
			inputState.keyStates.clear();
			inputState.mouseButtonStates.clear();
			inputState.mouseX = inputState.mouseY = 0.0;
			inputState.lastMouseX = inputState.lastMouseY = 0.0;
			inputState.scrollX = inputState.scrollY = 0.0;
			return;
		}

		if (inputCaptured) {
			inputState.keyStates = keyStates;
		} else {
			inputState.keyStates.clear();
		}

		if (mouseEntered) {
			inputState.mouseButtonStates = mouseButtonStates;
			inputState.mouseX = mouseX;
			inputState.mouseY = mouseY;
			inputState.lastMouseX = lastMouseX;
			inputState.lastMouseY = lastMouseY;
			inputState.scrollX = scrollX;
			inputState.scrollY = scrollY;
		} else {
			inputState.mouseButtonStates.clear();
			inputState.mouseX = inputState.mouseY = 0.0;
			inputState.lastMouseX = inputState.lastMouseY = 0.0;
			inputState.scrollX = inputState.scrollY = 0.0;
		}
			   });

	while (!keyReleaseCheck.empty()) {
		auto key = static_cast<Input::Key>(keyReleaseCheck.front());
		if (keyStates[key] == Input::State::JustReleased)
			keyStates[key] = Input::State::Released;
		if (keyStates[key] == Input::State::JustPressed)
			keyStates[key] = Input::State::Pressed;
		keyReleaseCheck.pop();
	}

	while (!buttonReleaseCheck.empty()) {
		auto button = static_cast<Input::MouseButton>(buttonReleaseCheck.front());
		if (mouseButtonStates[button] == Input::State::JustReleased)
			mouseButtonStates[button] = Input::State::Released;
		if (mouseButtonStates[button] == Input::State::JustPressed)
			mouseButtonStates[button] = Input::State::Pressed;
		buttonReleaseCheck.pop();
	}

	if (mouseMoved) {
		lastMouseX = mouseX;
		lastMouseY = mouseY;
		mouseMoved = false;
	}



}

void Imp::InputSystem::cleanup(entt::registry& registry)
{
	auto group = registry.group<InputStateComponent>();
	group.each([this](auto entity, InputStateComponent& inputState) {
		inputState.keyStates.clear();
		inputState.mouseButtonStates.clear();
		inputState.mouseX = inputState.mouseY = 0.0;
		inputState.lastMouseX = inputState.lastMouseY = 0.0;
		inputState.scrollX = inputState.scrollY = 0.0;
			   });
	keyStates.clear();
	mouseButtonStates.clear();
	keyReleaseCheck = std::queue<uint32_t>();
	buttonReleaseCheck = std::queue<uint32_t>();
	//registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<KeyEvent>().disconnect<&InputSystem::OnKeyEvent>(*this);

	std::cout << "Input System Cleaned" << '\n';
	Debug::Info("Input System Cleaned");
	Debug::Info("keyStates.size: {}, MouseButtonStates.size: {}", keyStates.size(), mouseButtonStates.size());


}
