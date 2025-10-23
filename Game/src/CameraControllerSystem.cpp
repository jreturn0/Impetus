#include "Components/CameraComponent.h"
#include "Components/CameraControllerComponent.h"
#include "Components/CameraFollowComponent.h"
#include "Components/InputStateComponent.h"
#include "Components/ModelComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/Tags.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "ConfigSystem.h"
#include "core/ComputeEffect.h"
#include "CtxRef.h"
#include "InputCodes.h"
#include "Renderer.h"
#include "Systems/CameraControllerSystem.h"
#include "window/Window.h"
#include <Components/CameraLookAtComponent.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
namespace {
    auto cfg_cameraSensitivity = utl::ConfigValueRef("input.camera_sensitivity", 1.0f);

    inline float AngleWrap(float radians) {

       return glm::mod(radians + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
    }

    inline float AngleLerp(float a, float b, float t) {
        float delta = AngleWrap(b - a);
        return AngleWrap(a + delta * glm::clamp(t, 0.f, 1.f));
    }

    template <typename T>
        requires std::is_arithmetic_v<T>
    T Lerp(T a, T b, float t) {
        return a + (b - a) * glm::clamp(t, 0.f, 1.f);
    }
}

void CameraControllerSystem::initialize(entt::registry& registry)
{
    // Add CameraControllerComponent to entity with active camera if it doesn't have one
    auto group = registry.view<imp::ActiveCameraTag>();
    bool found = false;
    group.each([&](auto entity) {
        if (found)
            return;
        if (!registry.any_of<CameraControllerComponent>(entity)) {
            registry.emplace<CameraControllerComponent>(entity);
        }
        });

}

void CameraControllerSystem::update(entt::registry& registry, const float deltaTime)
{
    for (auto group = registry.view<imp::TransformComponent, imp::CameraComponent, CameraControllerComponent, imp::ActiveCameraTag>();
        auto&& [entity, transform, camera, controller] : group.each()) {

        auto&& inputGroup = registry.view<imp::InputStateComponent>();
        if (inputGroup.empty()) return;
        auto& input = registry.get<imp::InputStateComponent>(*inputGroup.begin());

        auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
        if (renderer.getWindow().getCursorMode() == imp::Window::CursorMode::Disabled) {
            if (registry.view<imp::CameraLookAtComponent>().empty()) {

                const glm::vec2 mouseDelta = input.getMouseDelta();
                const glm::vec2 keyLook{
                    Input::GetAxis<float>(input.keyStates[Input::Key::Up],   input.keyStates[Input::Key::Down]),
                    Input::GetAxis<float>(input.keyStates[Input::Key::Left], input.keyStates[Input::Key::Right])
                };

                const float sens = static_cast<float>(cfg_cameraSensitivity.getCopy());
                glm::vec2 deltaDeg{
                    (-mouseDelta.y + keyLook.x) * sens,
                    (mouseDelta.x + keyLook.y) * sens
                };

                constexpr float maxDelta = glm::two_pi<float>()*3;
                deltaDeg = glm::clamp(deltaDeg, -maxDelta, maxDelta);

                controller.targetPitch += glm::radians(deltaDeg.x);
                controller.targetPitch = glm::clamp(controller.targetPitch,
                    -glm::half_pi<float>() + 0.02f,
                    glm::half_pi<float>() - 0.02f);

                controller.targetYaw += glm::radians(deltaDeg.y);
                controller.targetYaw = AngleWrap(controller.targetYaw);
                float t = 1.0f - std::exp(-controller.smoothSpeed * deltaTime);
                if (controller.smooth) {
                    controller.pitch = AngleLerp(controller.pitch, controller.targetPitch, t);
                    controller.yaw = AngleLerp(controller.yaw, controller.targetYaw, t);
                }
                else {
                    controller.pitch = controller.targetPitch;
                    controller.yaw = controller.targetYaw;
                }
                const glm::quat yawQ = glm::angleAxis(controller.yaw, glm::vec3{ 0.f, 1.f, 0.f });
                const glm::quat pitchQ = glm::angleAxis(controller.pitch, glm::vec3{ 1.f, 0.f, 0.f });
                transform.rotation = glm::normalize(yawQ * pitchQ);

            }
        }

        auto&& view = registry.group<>(entt::get<CameraFollowComponent, imp::TransformComponent>);
        if (!view.empty()) {
            auto [follow, targetTransform] = view.get<CameraFollowComponent, imp::TransformComponent>(*view.begin());

            if (follow.orbit) {
                const float distance = std::max(follow.orbitDistance, 0.001f);

                const glm::quat yawQ = glm::angleAxis(controller.yaw, glm::vec3{ 0.f, 1.f, 0.f });
                const glm::vec3 right = glm::rotate(yawQ, glm::vec3{ 1.f, 0.f, 0.f });
                const glm::quat pitchQ = glm::angleAxis(controller.pitch, right);
                const glm::quat orbitQ = glm::normalize(pitchQ * yawQ); 

                const glm::vec3 forward = glm::normalize(glm::rotate(orbitQ, glm::vec3{ 0.f, 0.f, -1.f }));
                const glm::vec3 desiredPosition = targetTransform.position - forward * distance;


                transform.position = glm::mix(transform.position, desiredPosition, follow.smoothSpeed * deltaTime);
                transform.rotation = orbitQ;
            }
            else {
                glm::vec3 desiredPosition = targetTransform.position + follow.offset;
                transform.position = glm::mix(transform.position, desiredPosition, follow.smoothSpeed * deltaTime);
            }
        }
        else {
            auto moveDir = glm::vec3{ 0.f };
            moveDir.x += Input::GetAxis<float>(input.keyStates[Input::Key::A], input.keyStates[Input::Key::D]);
            moveDir.y += Input::GetAxis<float>(input.keyStates[Input::Key::LeftControl], input.keyStates[Input::Key::Space]);
            moveDir.z += Input::GetAxis<float>(input.keyStates[Input::Key::W], input.keyStates[Input::Key::S]);



            if (glm::length2(moveDir) > std::numeric_limits<float>::epsilon()) {
                float speed = Input::IsPressed(input.keyStates[Input::Key::LeftShift]) ? controller.moveBoostSpeed : controller.moveSpeed;
                transform.position += (transform.rotation * glm::normalize(moveDir)) * speed * deltaTime;
            }
        }


    }
}

