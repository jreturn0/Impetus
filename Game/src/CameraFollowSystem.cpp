#include "Systems/CameraFollowSystem.h"
#include <Components/CameraFollowComponent.h>
#include <Components/Tags.h>
#include <Components/TransformComponent.h>

void CameraFollowSystem::update(entt::registry& registry, const float deltaTime)
{
    auto camGroup = registry.group<>(entt::get<imp::TransformComponent, CameraFollowComponent>);
    auto targetGroup = registry.view<imp::TransformComponent, imp::ActiveCameraTag>();
    if (camGroup.empty() || (targetGroup.begin() == targetGroup.end())) return;
    auto&& [camTransform, camFollow] = camGroup.get<imp::TransformComponent, CameraFollowComponent>(*camGroup.begin());
    auto&& targetTransform = targetGroup.get<imp::TransformComponent>(*targetGroup.begin());
    glm::vec3 desiredPosition = targetTransform.position + camFollow.offset;
    camTransform.position = glm::mix(camTransform.position, desiredPosition, camFollow.smoothSpeed * deltaTime);
}
