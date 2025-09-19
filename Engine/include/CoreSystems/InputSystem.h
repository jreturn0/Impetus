#pragma once
#include <queue>
#include <thread>
#include <GLFW/glfw3.h>

#include "BasicEvents.h"
#include "System.h"



namespace imp {
	class InputSystem final : public System
	{
	private:
		bool initalized = false;
		double test = 0.;
		std::unordered_map<Input::Key,Input::State> keyStates;
		std::unordered_map<Input::MouseButton, Input::State> mouseButtonStates;
		double mouseX{ 0.}, mouseY{ 0.}, lastMouseX{ 0.}, lastMouseY{0.};
		double scrollX{ 0.}, scrollY{ 0.};
		bool mouseMoved = false;
		bool keyPressed = false;
		bool mouseButtonPressed = false;
		bool inputCaptured = true;
		bool mouseEntered = true;
		bool mouseCaptured = false;
		std::queue<uint32_t> keyReleaseCheck;
		std::queue<uint32_t> buttonReleaseCheck;


		inline static unsigned idCounter = 0;
		unsigned id = idCounter++;

		
	public:
		void initialize(entt::registry& registry) override;
		void OnKeyEvent(KeyEvent keyEvent) ;
		void OnMouseMoveEvent( MouseMoveEvent mouseMoveEvent);
		void OnMouseButtonEvent(MouseButtonEvent mouseButtonEvent);
		void OnMouseScrollEvent( MouseScrollEvent mouseScrollEvent);
		void OnMouseEnterEvent(MouseEnterEvent mouseEnterEvent);
		void OnWindowFocusEvent(WindowFocusEvent windowFocusEvent);



		void update(entt::registry& registry, const float deltaTime) override;

		void cleanup(entt::registry& registry) override;
	};
}