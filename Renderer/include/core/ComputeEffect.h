#pragma once
#include <string>
#include <glm/vec4.hpp>
#include "utils/QuickMacros.h"

namespace Imp {
	class ComputeEffect
	{
	public:
		struct PushConstants
		{
			glm::vec4 data1 = { 1.f,0.f,0.f,1.f };
			glm::vec4 data2 = { 0.f,1.f,0.f,1.f };
			glm::vec4 data3 = { 0.f,0.f,1.f,1.f };
			glm::vec4 data4 = { 0.f,0.f,0.f,1.f };

			glm::vec4 & operator[](size_t index)
			{
				switch (index)
				{
				case 0:
					return data1;
				case 1:
					return data2;
				case 2:
					return data3;
				case 3:
					return data4;
				default:
					return data1;
				}
			}
		};
	protected:
		PushConstants pushes;
		std::string name;

	public:
		ComputeEffect(const std::string& name) : name(name) {}
		ComputeEffect() = default;
		~ComputeEffect() = default;
		DISABLE_COPY_AND_MOVE(ComputeEffect);



		PushConstants& getPushConstants() { return pushes; }
		const PushConstants& getPushConstants() const { return pushes; }
		void setPushConstants(const PushConstants& data) { pushes = data; }
		void setPushConstants(const glm::vec4& data1, const glm::vec4& data2, const glm::vec4& data3, const glm::vec4& data4) { pushes = { data1, data2, data3, data4 }; }
		const std::string& getName() const { return name; }
		const char* getName() { return name.c_str(); }

	};

}