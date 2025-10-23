#include "Clock.h"
#include "Components/ModelComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/TransformComponent.h"
#include "CoreSystems/TransformSystem.h"
#include "CtxRef.h"
#include "Debug.h"
#include "events/BasicEvents.h"
#include "Physics/Physics.h"
#include "ThreadPool.h"
#include <execution>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>



void imp::TransformSystem::initialize(entt::registry& registry)
{
    time = &registry.ctx().get<CtxRef<imp::Clock>>().get();
}
void imp::TransformSystem::update(entt::registry& registry, const float deltaTime)
{
    if (time == nullptr) {
        Debug::Error("Time is nullptr");
        return;
    }

    const double ratio = time->getAccumulatedTime() / time->getFixedStep();
    const float alpha = static_cast<float>(std::clamp(ratio, 0.0, 1.0));

    auto transformGroup = registry.group<imp::TransformComponent>(entt::get<imp::ModelComponent>);
    auto& physicsPrev = registry.storage<imp::TransformComponent>("physics_prev"_hs);
    auto& physicsCurr = registry.storage<imp::TransformComponent>("physics"_hs);

    for (auto&& [entity, transform, model] : transformGroup.each()) {
        const glm::vec3 scale = transform.scale;

        if (physicsPrev.contains(entity) && physicsCurr.contains(entity)) {
            const auto& prevT = physicsPrev.get(entity);
            const auto& currT = physicsCurr.get(entity);
            glm::vec3 interpPos = glm::mix(prevT.position, currT.position, alpha);
            glm::quat interpRot = glm::normalize(glm::slerp(prevT.rotation, currT.rotation, alpha));
            transform.position = interpPos;
            transform.rotation = interpRot;
        }
        else if (physicsCurr.contains(entity)) {
            const auto& currT = physicsCurr.get(entity);
            transform.position = currT.position;
            transform.rotation = currT.rotation;
        }
        const glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform.position);
        const glm::mat4 rotation = glm::toMat4(transform.rotation);
        const glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

        model.localTransform = translation * rotation * scaling;
    }
}



