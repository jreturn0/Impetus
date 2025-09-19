#pragma once
#include <entt/entity/entity.hpp>

#include "PhysicsCommon.h"
#include "utils/QuickMacros.h"
#include <Physics/PhysicsImpls.h>

#include "BodyActivationListener.h"
#include "ContactListener.h"
#include "BodyIDHash.h"
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>

#include <Jolt/Physics/Collision/CollisionCollector.h>
namespace imp::Phys {





	class Physics
	{
	private:


		std::unique_ptr<JPH::Factory> factory;

		std::unique_ptr < JPH::TempAllocatorImpl> tempAllocator;
		std::unique_ptr< JPH::JobSystemThreadPool> jobSystem;

		BPLayerInterfaceImpl broadPhaseLayerInterface;
		ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;

		ObjectLayerPairFilterImpl objectLayerPairFilter;

		std::unique_ptr<JPH::PhysicsSystem> physicsSystem;

		BodyActivationListener bodyActivationListener;
		ContactListener contactListener;

		std::array< JPH::BodyID, MAX_BODIES> destroyList;
		size_t destroyListSize = 0;

		//JPH::BodyID custom hash function


		std::queue< std::function<void()>> deferredEvents;

		std::unordered_map<entt::entity, JPH::BodyID> entityToBodyMap;
		std::unordered_map<JPH::BodyID, entt::entity, BodyIDHash> bodyToEntityMap;
		std::unordered_map<entt::entity, std::unordered_set<entt::entity>> collidingEntitiesMap;






	public:



		auto&& getCollidingEntities(entt::entity owner)
		{
			return collidingEntitiesMap[owner];
		}

		auto& getContactListener() { return contactListener; }

		auto getBody(entt::entity owner) const
		{
			return entityToBodyMap.at(owner);
		}
		bool hasBody(entt::entity owner) const
		{
			return entityToBodyMap.contains(owner);
		}
		bool tryGetBody(entt::entity owner, JPH::BodyID& body) const
		{
			if (hasBody(owner)) {
				body = getBody(owner);
				return true;
			}
			return false;
		}





		entt::entity getEntity(JPH::BodyID body) const
		{
			if (!bodyToEntityMap.contains(body)) {
				Debug::Error("Body not found\n");
				return entt::null;
			}
			return bodyToEntityMap.at(body);
		}
		bool hasEntity(JPH::BodyID body) const
		{
			if (!bodyToEntityMap.contains(body)) {
				return false;
			}
			return bodyToEntityMap.contains(body);
		}

		bool tryGetEntity(JPH::BodyID body, entt::entity& owner) const
		{
			if (bodyToEntityMap.contains(body)) {
				owner = getEntity(body);
				return true;
			}
			return false;
		}

		RayHit getMouseWorldPosition(const glm::vec2& mousePosition, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec2& viewport) const
		{
			float x = (2.0f * mousePosition.x) / viewport.x - 1.0f;
			// Invert the Y-axis for Vulkan
			float y = (2.0f * mousePosition.y) / viewport.y - 1.0f;
			glm::vec4 rayClip(x, y, -1.0f, 1.0f);  // NDC coordinates

			// Step 2: Convert to camera space (eye space)
			glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
			rayEye.z = -1.0f; // Ensure the ray direction is correct in eye space
			rayEye.w = 0.0f;  // No translation component

			// Step 3: Convert to world space using the inverse of the view matrix
			glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(viewMatrix) * rayEye));

			// Step 4: Define the ray starting from the camera's position (extracted from the view matrix)
			glm::vec3 cameraPosition = glm::vec3(glm::inverse(viewMatrix)[3]);

			return rayCast(cameraPosition, rayWorld, 1000.0f);

		}

		RayHit rayCast(const glm::vec3& origin, const glm::vec3& direction, const float distance) const
		{
			const JPH::RRayCast ray{ ToJPH(origin), ToJPH(direction)*distance };
			JPH::RayCastResult hit{};
			//	JPH::BroadPhaseLayerFilter layerFilter{};
			const bool didHit = physicsSystem->GetNarrowPhaseQuery().CastRay(ray, hit);

			if (!didHit)
			{
				return { didHit };
			}
			const auto point = ray.GetPointOnRay(hit.mFraction);
			return RayHit{
				didHit,
				getEntity(hit.mBodyID),
				hit.mBodyID,
				ToGLM(point),
				ToGLM(physicsSystem->GetBodyLockInterfaceNoLock().TryGetBody(hit.mBodyID)->GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, point)),
				hit.mFraction,
			};
		}


		explicit Physics(entt::dispatcher& eventDispatcher);
		explicit Physics();
		~Physics();
		DISABLE_COPY_AND_MOVE(Physics)

			auto& getBodyInterface()const { return physicsSystem->GetBodyInterface(); }
		auto& getBodyInterface() { return physicsSystem->GetBodyInterface(); }
		auto& getSystem()const { return *physicsSystem; }

		void setGravity(const glm::vec3& gravity) const
		{
			physicsSystem->SetGravity(ToJPH(gravity));
		}

		glm::vec3 getGravity() const
		{
			return ToGLM(physicsSystem->GetGravity());
		}

		void optimizeBroadPhase() const
		{
			physicsSystem->OptimizeBroadPhase();
		}

		JPH::BodyID createBody(entt::entity owner, const JPH::Shape* shape, const glm::vec3& position, const  glm::quat& rotation, JPH::EMotionType motionType, ObjectLayer layer, JPH::EActivation activation = JPH::EActivation::Activate)
		{

			JPH::BodyCreationSettings settings(shape, ToJPH(position), ToJPH(rotation), motionType, (layer));
			settings.mAllowDynamicOrKinematic = true;

			auto&& body = physicsSystem->GetBodyInterface().CreateAndAddBody(settings, activation);
			if (body.IsInvalid()) {
				Debug::Error("Failed to create body\n");
			}
			entityToBodyMap[owner] = body;
			bodyToEntityMap[body] = owner;
			return 	body;// physicsSystem->GetBodyInterface().CreateAndAddBody(settings, JPH::EActivation::Activate);

		}

		void destroyBody(entt::entity owner)
		{
			destroyBody(entityToBodyMap[owner]);
		}

		void destroyBodies()
		{
			if (destroyListSize == 0)
				return;
			physicsSystem->GetBodyInterface().RemoveBodies(&*destroyList.begin(), static_cast<uint32_t>(destroyListSize));
			physicsSystem->GetBodyInterface().DestroyBodies(&*destroyList.begin(), static_cast<uint32_t>(destroyListSize));
			destroyListSize = 0;
		}

		void destroyBody(JPH::BodyID body)
		{
			entityToBodyMap.erase(bodyToEntityMap[body]);
			bodyToEntityMap.erase(body);
			destroyList[destroyListSize] = (body);
			++destroyListSize;
		}

		void update(float deltaTime)
		{
			while (!deferredEvents.empty()) {
				deferredEvents.front()();
				deferredEvents.pop();
			}
			destroyBodies();
			physicsSystem->Update(deltaTime, COLLISION_STEPS, &*tempAllocator, &*jobSystem);
		}




	};
}

