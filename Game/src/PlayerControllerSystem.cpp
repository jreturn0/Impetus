#include "Components/CameraControllerComponent.h"
#include "Components/GameTagComponents.h"
#include "Components/InputStateComponent.h"
#include "Components/MoveDirectionComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/SpeedComponent.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "Systems/PlayerControllerSystem.h"
#include <glm/gtx/quaternion.hpp>

void PlayerControllerSystem::update(entt::registry& registry, const float deltaTime)
{
    auto&& group = registry.group<PlayerTag>(entt::get<MoveDirectionComponent>);
    if (group.empty()) return;

    auto&& direction = group.get<MoveDirectionComponent>(group.front());

    auto&& view = registry.view<imp::InputStateComponent>();
    auto&& input = view.get<imp::InputStateComponent>(view.front());


    auto strafeAndFwd = glm::vec3{ 0.f };
    strafeAndFwd.x += Input::GetAxis(input.keyStates[Input::Key::A], input.keyStates[Input::Key::D]);
    strafeAndFwd.z += Input::GetAxis(input.keyStates[Input::Key::W], input.keyStates[Input::Key::S]);

    auto cam = registry.view<imp::ActiveCameraTag,CameraControllerComponent>();
    if (cam.begin() != cam.end()) {
        auto&& [controller, transform] = registry.get<CameraControllerComponent, imp::TransformComponent>(*cam.begin());        
        const auto rotation = glm::angleAxis(controller.yaw, glm::vec3(0.f, 1.f, 0.f));
        strafeAndFwd = rotation * strafeAndFwd;
    }

    direction.direction = strafeAndFwd;
}
