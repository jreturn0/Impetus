#include "ComponentInfoRegistry.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "CoreSystems/CameraSystem.h"
#include "CtxRef.h"
#include "Debug.h"
#include "Renderer.h"
#include "window/Window.h"
#include <Components/CameraComponent.h>
#include <Components/CameraLookAtComponent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


void imp::CameraSystem::OnActiveCameraTagCreate(entt::registry& registry, entt::entity entity)
{
    auto camTags = registry.view<imp::ActiveCameraTag>();
    std::vector<entt::entity> entities;
    for (auto&& [otherEntity] : camTags.each()) {
        if (entity != otherEntity) {
            entities.push_back(otherEntity);
        }
    }
    for (auto otherEntity : entities) {
        registry.remove<imp::ActiveCameraTag>(otherEntity);
    }
}

void imp::CameraSystem::initialize(entt::registry& registry)
{
    registry.on_construct<ActiveCameraTag>().connect<&imp::CameraSystem::OnActiveCameraTagCreate>(*this);

    auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
    const auto& window = renderer.getWindow();
    const auto width = window.getWidth();
    const auto height = window.getHeight();
    const float ratio = static_cast<float>(width) / static_cast<float>(height);
    auto&& view = registry.view<imp::CameraComponent>();
    entt::entity cameraEntity{};
    if (view.empty()) {

        Debug::Info("Creating Camera");
        cameraEntity = registry.create();
        registry.emplace<imp::TransformComponent>(cameraEntity, glm::vec3{ 0.f,0.f,15.f }, glm::quat{ 1.f,0.f,0.f,0.f }, glm::vec3{ 1.f,1.f,1.f });
        registry.emplace<imp::CameraComponent>(cameraEntity);
        registry.emplace<ActiveCameraTag>(cameraEntity);
    }
    else {
        cameraEntity = *view.begin();
        if (!registry.any_of<ActiveCameraTag>(cameraEntity)) {
            registry.emplace<ActiveCameraTag>(cameraEntity);
        }
    }

    auto& cam = registry.get<imp::CameraComponent>(cameraEntity);
    renderer.getSceneData().proj = glm::perspective(glm::radians(cam.fov), ratio, cam.nearPlane, cam.farPlane);
}

void imp::CameraSystem::update(entt::registry& registry, const float deltaTime)
{
    const auto camTags = registry.group<>(entt::get< imp::CameraComponent, imp::TransformComponent, imp::ActiveCameraTag>);
    if (camTags.empty()) {
        Debug::Error("No active camera");
        return;
    }
    const auto entity = camTags.front();
    auto&& [cam, transform] = camTags.get<CameraComponent, TransformComponent>(entity);
    auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
    auto& sceneData = renderer.getSceneData();
    const auto& window = renderer.getWindow();
    const auto width = window.getWidth();
    const auto height = window.getHeight();
    const float ratio = static_cast<float>(width) / static_cast<float>(height);
    auto&& lookAtTags = registry.group<CameraLookAtComponent>(entt::get< TransformComponent>);



    glm::vec3 up{};
    glm::vec3 targetPos{};
    if (!lookAtTags.empty()) {
        auto&& [lookAt, transform] = lookAtTags.get<CameraLookAtComponent, TransformComponent>(lookAtTags.front());
        if (lookAt.smoothing> 0.01f) {
            targetPos = glm::mix(lookAt.previousTargetPosition, transform.position, glm::clamp(lookAt.smoothing * deltaTime, 0.0f, 1.0f));
        }
        else {
            targetPos = transform.position;
        }
        up = GetWorldUp();
    }
    else {
        targetPos = transform.position + GetTransformForward(transform);
        up = GetTransformUp(transform);
    }
    sceneData.inverseView = glm::lookAt(transform.position, targetPos, up);
    //TODO: caching would be good here
    if (cam.type == imp::CameraType::Orthographic) {
        const float left = -cam.fov * ratio;
        const float right = cam.fov * ratio;
        const float bottom = -cam.fov;
        const float top = cam.fov;
        sceneData.proj = glm::ortho(left, right, bottom, top, cam.nearPlane, cam.farPlane);
        sceneData.proj[1][1] *= -1; // Invert Y for Vulkan-style NDC

    }
    else {
        sceneData.proj = glm::perspective(glm::radians(cam.fov), ratio, cam.nearPlane, cam.farPlane);
        sceneData.proj[1][1] *= -1; // Invert Y for Vulkan-style NDC
    }

    sceneData.viewproj = sceneData.proj * sceneData.inverseView;
    sceneData.inverseView = glm::inverse(sceneData.inverseView); 
}

