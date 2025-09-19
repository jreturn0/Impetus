#pragma once
#include "utils/VKCommon.hpp"
#include <utils/Forwards.h>

namespace imp::gfx
{
	class GUI
	{

	public:
		GUI(VulkanContext& context, Window& window, vk::Format drawFormat);
		~GUI();
		GUI(const GUI&) = delete; 
		GUI& operator=(const GUI&) = delete; 
		GUI(GUI&&) = delete; 
		GUI& operator=(GUI&&) = delete;;


		void startFrame();
		void render(vk::CommandBuffer cmd);
		void endFrame();
	private:

		vk::raii::DescriptorPool m_descriptorPool{ nullptr };
	};
}
