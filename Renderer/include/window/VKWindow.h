#pragma once
#include "window/Window.h"
#include "utils/VKCommon.hpp"
namespace Imp::Render {

		class VKWindow final : public Window
		{
		private:
			vk::UniqueSurfaceKHR surface;
			vk::UniqueSurfaceKHR createSurface(const vk::Instance& instance) const;
			friend class VKRenderer;
		public:

			VKWindow(uint32_t _width, uint32_t _height, const char* _title,const  vk::Instance& instance);
			VKWindow(uint32_t _width, uint32_t _height, const char* _title);
			void setSurface( const vk::Instance& instance);

			inline const vk::SurfaceKHR& getSurface() const { return *surface; }
			inline vk::Extent2D getExtent() const { return { width, height }; }
			
		};
	
}
