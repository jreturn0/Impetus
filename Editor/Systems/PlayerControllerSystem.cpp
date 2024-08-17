#include "PlayerControllerSystem.h"

#include "../Components/GameTagComponents.h"
#include "../Components/MoveDirectionComponent.h"
#include "../Components/SpeedComponent.h"
#include "Components/InputStateComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/TransformComponent.h"

void PlayerControllerSystem::update(entt::registry& registry, const float deltaTime)
{
	auto&& group = registry.group<PlayerTag>(entt::get<MoveDirectionComponent>);
	if (group.empty()) return;

	auto&& direction = group.get<MoveDirectionComponent>(group.front());

	auto&& view = registry.view<Imp::InputStateComponent>();
	auto&& input = view.get<Imp::InputStateComponent>(view.front());


	auto strafe = glm::vec3{ 0.f };
	strafe += Input::IsPressed(input.keyStates[Input::Key::A]) ? -glm::vec3{ 1.f ,0.f,0.f } : glm::vec3{ 0.f };
	strafe += Input::IsPressed(input.keyStates[Input::Key::D]) ? glm::vec3{ 1.f ,0.f,0.f } : glm::vec3{ 0.f };

	auto forward = glm::vec3{ 0.f };
	forward += Input::IsPressed(input.keyStates[Input::Key::W]) ? -glm::vec3{ 0.f,0.f,1.f } : glm::vec3{ 0.f };
	forward += Input::IsPressed(input.keyStates[Input::Key::S]) ? glm::vec3{ 0.f,0.f,1.f } : glm::vec3{ 0.f };
	direction.direction = strafe + forward;
}
