#include "Components/AimTargetComponent.h"
#include "Components/GameTagComponents.h"
#include "Components/InputStateComponent.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "CtxRef.h"
#include "Physics/Physics.h"
#include "Renderer.h"
#include "Systems/AimingSystem.h"
#include "window/Window.h"
#include <Jolt/Physics/Collision/RayCast.h>

void AimingSystem::initialize(entt::registry& registry)
{
    if (registry.view<AimTargetComponent>().empty()) {
        const auto entity = registry.create();

        registry.emplace<AimTargetComponent>(entity);
        registry.emplace<imp::TransformComponent>(entity);
    }
}

void AimingSystem::update(entt::registry& registry, const float deltaTime)
{
    {
        auto aimView = registry.view<AimTargetComponent, imp::TransformComponent>();
        auto camGroup = registry.group<>(entt::get<imp::TransformComponent, imp::ActiveCameraTag>);
        auto inputView = registry.view<imp::InputStateComponent>();
        auto& physics = registry.ctx().get<CtxRef<imp::phys::Physics>>().get();
        auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
        const auto& sceneData = renderer.getSceneData(); 
        auto windowSize = renderer.getWindow().getSizeVec();
        if (inputView.empty()) return;
        const auto inputEntity = *inputView.begin();
        const auto& input = inputView.get<imp::InputStateComponent>(inputEntity);

        if (aimView.begin() == aimView.end()) return;
        const auto aimEntity = *aimView.begin();
        auto& aimTarget = aimView.get<AimTargetComponent>(aimEntity);
        auto& aimTransform = aimView.get<imp::TransformComponent>(aimEntity);

        


        camGroup.each([&](auto camEntity, auto& camTransform) {
            glm::vec3 dir;
            glm::vec3 basePos;

            if (aimTarget.useCameraPlayerRelative) {
                auto view = registry.view<PlayerTag, imp::TransformComponent>();
                if (view.begin() != view.end()) {
                    basePos = view.get<imp::TransformComponent>(*view.begin()).position;
                }
                else {
                    basePos = camTransform.position;
                }
                dir = GetTransformForward(camTransform);
            }
            else if (auto hit = physics.getMouseWorldPosition(
                input.getMousePosition(), sceneData.inverseView, sceneData.proj, windowSize))
            {
                basePos = hit.point;
                dir = hit.normal;
            }
            else {
                basePos = aimTransform.position;
                dir = GetTransformForward(camTransform);
            }

            if (glm::length2(dir) > 0.0f)
                dir = glm::normalize(dir);
            else
                dir = glm::vec3(0, 0, 1);

            if (aimTarget.lockY)
                dir.y = 0.0f; 

            if (glm::length2(dir) > 0.0f)
                dir = glm::normalize(dir) * aimTarget.normalMultiplier;

            glm::vec3 aimPosition = basePos + dir + aimTarget.offset;

            if (aimTarget.lockY)
                aimPosition.y = aimTarget.yHeight;

            aimTransform.position = aimPosition;
            });


    }


}


