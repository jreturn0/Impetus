#include "CoreSystems/CameraSystem.h"
#include <Components/CameraComponent.h>

#include "ComponentInfoRegistry.h"
#include "CtxRef.h"
#include "Renderer.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "window/Window.h"




void Imp::CameraSystem::OnActiveCameraTagCreate(entt::registry& registry, entt::entity entity)
{
	auto camTags = registry.view<Imp::ActiveCameraTag>();
	std::vector<entt::entity> entities;
	for (auto&& [otherEntity] : camTags.each()) {
		if (entity != otherEntity) {
			entities.push_back(otherEntity);
		}
	}
	for (auto otherEntity : entities) {
		registry.remove<Imp::ActiveCameraTag>(otherEntity);
	}
}

void Imp::CameraSystem::initialize(entt::registry& registry)
{
	registry.on_construct<ActiveCameraTag>().connect<&Imp::CameraSystem::OnActiveCameraTagCreate>(*this);
	if (registry.view<Imp::CameraComponent>().empty()) {

		Debug::Info("Creating Camera");
		const auto cameraEntity = registry.create();
		registry.emplace<Imp::TransformComponent>(cameraEntity, glm::vec3{ 0.f,0.f,-15.f }, glm::quat{ 0.f,0.f,0.f,0.f }, glm::vec3{ 1.f,1.f,1.f });
		registry.emplace<Imp::CameraComponent>(cameraEntity);
		registry.emplace<ActiveCameraTag>(cameraEntity);
	}
}
glm::mat4 InterpolateMatrices(const glm::mat4& start, const glm::mat4& end, float t)
{
	// Extract translation
	glm::vec3 startTranslation = glm::vec3(start[3]);
	glm::vec3 endTranslation = glm::vec3(end[3]);

	// Interpolate translation
	glm::vec3 interpolatedTranslation = glm::mix(startTranslation, endTranslation, t);

	// Extract rotation
	glm::quat startRotation = glm::quat_cast(start);
	glm::quat endRotation = glm::quat_cast(end);

	// Interpolate rotation
	glm::quat interpolatedRotation = glm::slerp(startRotation, endRotation, t);

	// Construct the interpolated matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(interpolatedRotation);
	rotationMatrix[3] = glm::vec4(interpolatedTranslation, 1.0f);

	return rotationMatrix;
}
void Imp::CameraSystem::update(entt::registry& registry, const float deltaTime)
{

	{
		const auto camTags = registry.group<>(entt::get< Imp::CameraComponent, Imp::TransformComponent, Imp::ActiveCameraTag>);
		if (camTags.empty()) {
			Debug::Error("No active camera");
			return;
		}
		const auto entity = camTags.front();
		auto&& [cam, transform] = camTags.get<CameraComponent, TransformComponent>(entity);
		// Update the view matrix
		auto& renderer = registry.ctx().get<CtxRef<Imp::Render::Renderer>>().get();
		auto& sceneData = renderer.getSceneData();

		const auto& window = renderer.getWindow();
		const auto width = window.getWidth();
		const auto height = window.getHeight();
		const float ratio = static_cast<float>(width) / static_cast<float>(height);
		auto&& lookAtTags = registry.group<CameraLookAtTag>(entt::get< TransformComponent>);


		if (!lookAtTags.empty()) {
			auto&& target = lookAtTags.get<TransformComponent>(lookAtTags.front());
			sceneData.view = glm::lookAt(transform.position, target.position, glm::vec3{ 0.f,1.f,0.f });
		} else {
			sceneData.view = glm::lookAt(transform.position, transform.position + GetTransformForward(transform), GetTransformUp(transform));

		}



		// Construct the projection matrix
		if (cam.type == Imp::CameraType::Orthographic) {
			// Orthographic camera settings
			const float left = -cam.fov * ratio;
			const float right = cam.fov * ratio;
			const float bottom = -cam.fov;
			const float top = cam.fov;
			sceneData.proj = glm::ortho(left, right, bottom, top, cam.nearPlane, cam.farPlane);
			sceneData.proj[1][1] *= -1; // Invert Y for Vulkan-style NDC

		} else {
			sceneData.proj = glm::perspective(glm::radians(cam.fov), ratio, cam.nearPlane, cam.farPlane);
			sceneData.proj[1][1] *= -1; // Invert Y for Vulkan-style NDC
		}

		sceneData.viewproj = sceneData.proj * sceneData.view;

	}

}

void Imp::CameraSystem::cleanup(entt::registry& registry)
{}
