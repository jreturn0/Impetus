#pragma once
#include <glm/gtc/quaternion.hpp>

#include "CtxRef.h"
#include "InputCodes.h"
#include "Renderer.h"
#include "System.h"
#include "Components/Tags.h"
#include "Components/CameraComponent.h"
#include "Components/CameraFollowComponent.h"
#include "Components/TransformComponent.h"
#include "Components/InputStateComponent.h"
#include "Components/ModelComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/Tags.h"
#include "core/ComputeEffect.h"
#include "window/Window.h"

inline void RotateCamera(imp::CameraComponent& cam, imp::TransformComponent& tran, glm::vec2 input)
{

	// Update pitch and yaw
	cam.pitch -= input.y;
	cam.yaw += input.x;

	// Clamp the pitch to avoid flipping
	cam.pitch = glm::clamp(cam.pitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);

	// Create quaternions for pitch and yaw
	glm::quat pitchQuat = glm::angleAxis(cam.pitch, glm::vec3(1, 0, 0)); // Rotation around x-axis
	glm::quat yawQuat = glm::angleAxis(cam.yaw, glm::vec3(0, 1, 0)); // Rotation around y-axis

	// Combine pitch and yaw rotations
	glm::quat rotation = yawQuat * pitchQuat;

	// Update the transform's rotation
	tran.rotation = rotation;
}

class CameraControllerSystem final : public System
{


	void initialize(entt::registry& registry) override
	{
		auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
		renderer.getSceneData().proj = glm::perspective(glm::radians(80.0f), 16.0f / 9.0f, 0.01f, 1000.0f);
		//renderer.loadGLTF("Meshes/basic_rusty_meshes.glb");
		//renderer.loadGLTF("Meshes/structure.glb");
		renderer.loadComputeShader("sky2");

	}

	void update(entt::registry& registry, const float deltaTime) override
	{
		//Imp::Debug::Info("Camera Update");

		for (auto group = registry.view<imp::TransformComponent, imp::CameraComponent, imp::ActiveCameraTag>();
			 auto && [entity, transform, camera] : group.each()) {

			auto&& inputGroup = registry.view<imp::InputStateComponent>();
			if (inputGroup.empty()) {
				return;
			}
			auto input = registry.get<imp::InputStateComponent>(*inputGroup.begin());


			//Rotate camera if not in cursor mode and not looking at something

			auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
			if (renderer.getWindow().getCursorMode() == imp::Window::CursorMode::Disabled) {
				if (registry.view<imp::CameraLookAtTag>().empty()) {

					int rotatePitch = 0;
					rotatePitch += Input::IsPressed(input.keyStates[Input::Key::Left]) ? -1 : 0;
					rotatePitch += Input::IsPressed(input.keyStates[Input::Key::Right]) ? 1 : 0;

					int rotateYaw = 0;
					rotateYaw += Input::IsPressed(input.keyStates[Input::Key::Up]) ? -1 : 0;
					rotateYaw += Input::IsPressed(input.keyStates[Input::Key::Down]) ? 1 : 0;
					glm::vec2 rot{ rotatePitch,rotateYaw };

					RotateCamera(camera, transform, (input.getMouseDelta() + rot) * deltaTime);


				}
				
			}
			auto&& view = registry.group<>(entt::get< CameraFollowComponent, imp::TransformComponent>);
			if (!view.empty()) {
				auto [follow, targetTransform] = view.get<CameraFollowComponent, imp::TransformComponent>(*view.begin());
				transform.position = targetTransform.position + follow.offset;
			} else {
				auto strafe = glm::vec3{ 0.f };
				strafe += Input::IsPressed(input.keyStates[Input::Key::A]) ? -glm::vec3{ 1.f,0.f,0.f } : glm::vec3{ 0.f };
				strafe += Input::IsPressed(input.keyStates[Input::Key::D]) ? glm::vec3{ 1.f,0.f,0.f } : glm::vec3{ 0.f };

				auto forward = glm::vec3{ 0.f };
				forward += Input::IsPressed(input.keyStates[Input::Key::W]) ? -glm::vec3{ 0.f ,0.f,1.f } : glm::vec3{ 0.f };
				forward += Input::IsPressed(input.keyStates[Input::Key::S]) ? glm::vec3{ 0.f ,0.f,1.f } : glm::vec3{ 0.f };

				auto up = glm::vec3{ 0.f };
				up += Input::IsPressed(input.keyStates[Input::Key::Space]) ? glm::vec3{ 0.f,1.f,0.f } : glm::vec3{ 0.f };
				up += Input::IsPressed(input.keyStates[Input::Key::LeftControl]) ? -glm::vec3{ 0.f,1.f,0.f } : glm::vec3{ 0.f };


				if (glm::vec3 movement = strafe + up + forward; glm::length2(movement) > FLT_EPSILON) {
					if (Input::IsPressed(input.keyStates[Input::Key::LeftShift])) {
						transform.position += (transform.rotation * glm::normalize(movement)) * deltaTime * 20.f;
					} else
						transform.position += (transform.rotation * glm::normalize(movement)) * deltaTime * 5.f;
				}
			}

			// Normalize the quaternion
			transform.rotation = glm::normalize(transform.rotation);
			auto& pushes = renderer.getCurrentComputeEffect().getPushConstants();

			pushes[3].x = transform.rotation.x;
			pushes[3].y = transform.rotation.y;
			pushes[3].z = transform.rotation.z;
			pushes[3].w = transform.rotation.w;



		}
	}
};
