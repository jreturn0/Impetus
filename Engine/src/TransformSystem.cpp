#include "CoreSystems/TransformSystem.h"

#include <execution>

#include "Components/TransformComponent.h"
#include "Components/ModelComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/RelationshipComponent.h"
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include "Clock.h"


#include "BasicEvents.h"
#include "CtxRef.h"
#include "Debug.h"
#include "Physics/Physics.h"

namespace {
	void UpdateModelMatrix(entt::registry& registry, entt::entity entity)
	{
		auto* transform = registry.try_get<Imp::TransformComponent>(entity);
		auto* model = registry.try_get<Imp::ModelComponent>(entity);


		if (transform && model) {
			// Compute the local transform
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform->position);
			glm::mat4 rotation = glm::toMat4(transform->rotation);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform->scale);
			model->localTransform = translation * rotation * scale;

			auto* relationship = registry.try_get<Imp::RelationshipComponent>(entity);
			// Compute the global transform
			if (relationship && relationship->parent != entt::null) {
				if (const auto* parentModel = registry.try_get<Imp::ModelComponent>(relationship->parent)) {
					model->localTransform = parentModel->localTransform * model->localTransform;
				}
			} else {
				model->localTransform = model->localTransform;
			}

			// Update children

		}
	}

	//void SynchronizeTransform(entt::registry& registry, entt::entity entity, Imp::TransformComponent& transform, Imp::ModelComponent& model)
	//{

	//	// Compute the local transform
	//	glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform.position);
	//	glm::mat4 rotation = glm::toMat4(transform.rotation);
	//	glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform.scale);
	//	model.localTransform = translation * rotation * scale;

	//	//const auto* relationship = registry.try_get<Imp::RelationshipComponent>(entity);
	//	//// Compute the global transform
	//	//if (relationship && relationship->parent != entt::null) {
	//	//	if (const auto* parentModel = registry.try_get<Imp::ModelComponent>(relationship->parent)) {
	//	//		model.localTransform = parentModel->localTransform * model.localTransform;
	//	//	}
	//	//} else {
	//	//	model.localTransform = model.localTransform;
	//	//}

	//	// Update children
	//	//if (relationship) {
	//	//	for (auto child : relationship->children) {
	//	//		UpdateModelMatrix(registry, child);
	//	//	}
	//	//}


	//}
}

void Imp::TransformSystem::initialize(entt::registry& registry)
{
	//	observer.connect(registry, entt::collector.update<TransformComponent>());

	time = &registry.ctx().get<CtxRef<Imp::Clock>>().get();

}
void Imp::TransformSystem::update(entt::registry& registry, const float deltaTime)
{



	if (time == nullptr) {
		Debug::Error("Time is nullptr");
		return;
	}


	auto&& transformModelGroup = registry.group<>(entt::get< TransformComponent, ModelComponent>);
	auto&& physicsGroup = registry.group<>(entt::get<PhysicsBodyComponent, TransformComponent>);


	//physics interpolation factor
	constexpr double compare = 1.0 - DBL_EPSILON;
	double interpolationFactor = std::min(compare, time->getAccumulatedTime() / time->getFixedStep());
	auto&& physics = registry.ctx().get<CtxRef<Imp::Phys::Physics>>().get().getSystem().GetBodyInterfaceNoLock();

	Imp::ForEachParallel(physicsGroup, [&physics, &physicsGroup, &interpolationFactor](auto entity) {
		auto&& [physicsBody, transform] = physicsGroup.get<PhysicsBodyComponent, TransformComponent>(entity);
		JPH::Vec3 out1{}, out2;
		JPH::Quat out3{};
		physics.GetPositionAndRotation(physicsBody.id, out1, out3);
		transform.position = glm::mix(transform.position, Phys::ToGLM(out1), interpolationFactor);
		transform.rotation = glm::slerp(transform.rotation, Phys::ToGLM(out3), static_cast<float>(interpolationFactor));
						 });

	Imp::ForEachParallel(transformModelGroup, [&transformModelGroup](auto entity) {
		auto&& [transform, model]   = transformModelGroup.get<TransformComponent, ModelComponent>(entity);
		const glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform.position);
		const glm::mat4 rotation    = glm::toMat4(transform.rotation);
		const glm::mat4 scale       = glm::scale(glm::mat4(1.0f), transform.scale);
		model.localTransform       = translation * rotation * scale;
						 });

}

void Imp::TransformSystem::cleanup(entt::registry& registry) { System::cleanup(registry); }
