#pragma once
#include "CameraType.h"


namespace Imp
{

	struct CameraComponent
	{
		float fov = 85.0f;
		float nearPlane = 0.01f;
		float farPlane = 1000.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		CameraType type = CameraType::Perspective;
		bool interpolate = true;
	};
}
