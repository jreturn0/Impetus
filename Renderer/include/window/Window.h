#pragma once
#include <cstdint>
#include <glm/vec2.hpp>


#include "utils/QuickMacros.h"

struct GLFWwindow;
namespace Imp {
	class Window
	{
	public :
		enum class CursorMode
		{
			Normal = 0x00034001,
			Hidden = 0x00034002,
			Disabled = 0x00034003,
			Captured = 0x00034004
		};
	private:
		static GLFWwindow* CreateWindow(uint32_t width, uint32_t height, const char* title);
	protected:
		GLFWwindow* glfw;
		uint32_t width, height;
		const char* title;

		CursorMode cursorMode = CursorMode::Normal;

	public:



		void getSize(uint32_t& _width, uint32_t& _height) const;
		glm::vec2 getSize() const;
		inline uint32_t getWidth() const { return width; }
		inline uint32_t getHeight() const { return height; }
		inline const char* getTitle() const { return title; }
		inline GLFWwindow* getGLFW() const { return glfw; }
		inline CursorMode getCursorMode() const { return cursorMode; }

		DISABLE_COPY_AND_MOVE(Window);
		Window(uint32_t width, uint32_t height, const char* title);
		virtual ~Window();
		operator GLFWwindow* () { return glfw; }
		
		 void setCursorMode(CursorMode mode) ;



		//operator const GLFWwindow* ()
		//{
		//	return glfw;
		//}
	};
}
