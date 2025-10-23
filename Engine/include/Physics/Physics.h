#pragma once
#include "BodyActivationListener.h"
#include "BodyIDHash.h"
#include "ContactListener.h"
#include "PhysicsCommon.h"
#include "RayHit.h"
#include "utils/QuickMacros.h"
#include <entt/entity/entity.hpp>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollector.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Physics/PhysicsImpls.h>


namespace imp::phys {


    // The main physics class that manages the physics system ( implemented using Jolt Physics )
    class Physics
    {
    public:
        explicit Physics(entt::dispatcher& eventDispatcher);
        explicit Physics() = default;
        ~Physics();
        Physics(const Physics&) = delete;
        Physics& operator=(const Physics&) = delete;
        Physics(Physics&&) = delete;
        Physics& operator=(Physics&&) = delete;

        // Getters and Checkers

        inline const auto& getCollidingEntities(entt::entity owner) const { return m_collidingEntitiesMap.at(owner); }
        inline const auto& getContactListener() const { return m_contactListener; }
        inline auto& getContactListener() { return m_contactListener; }
        inline JPH::BodyID getBody(entt::entity owner) const { return m_entityToBodyMap.at(owner); }
        inline bool hasBody(entt::entity owner) const { return m_entityToBodyMap.contains(owner); }
        inline bool tryGetBody(entt::entity owner, JPH::BodyID& body) const { return hasBody(owner) ? (body = getBody(owner), true) : false; }
        inline entt::entity getEntity(JPH::BodyID body) const { return (!m_bodyToEntityMap.contains(body)) ? (Debug::Error("Body not found\n"), entt::null) : m_bodyToEntityMap.at(body); }
        inline bool hasEntity(JPH::BodyID body) const { return m_bodyToEntityMap.contains(body); }
        inline bool tryGetEntity(JPH::BodyID body, entt::entity& owner) const { return (m_bodyToEntityMap.contains(body)) ? (owner = getEntity(body), true) : false; }
        inline auto& getBodyInterface()const { return m_physicsSystem->GetBodyInterface(); }
        inline auto& getBodyInterface() { return m_physicsSystem->GetBodyInterface(); }
        inline auto& getSystem()const { return *m_physicsSystem; }
        inline void setGravity(const glm::vec3& gravity) const { m_physicsSystem->SetGravity(ToJPH(gravity)); }
        inline glm::vec3 getGravity() const { return ToGLM(m_physicsSystem->GetGravity()); }

        // Methods

        RayHit getMouseWorldPosition(const glm::vec2& mousePosition, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec2& viewport) const;
        RayHit rayCast(const glm::vec3& origin, const glm::vec3& direction, const float distance) const;
        void optimizeBroadPhase() const { m_physicsSystem->OptimizeBroadPhase(); }
        JPH::BodyID createBody(entt::entity owner, const JPH::Shape* shape, const glm::vec3& position, const  glm::quat& rotation, JPH::EMotionType motionType, ObjectLayer layer, JPH::EActivation activation = JPH::EActivation::Activate);
        void destroyBodies();
        void destroyBody(JPH::BodyID body);
        void destroyBody(entt::entity owner);
        void update(float deltaTime);


    private:        
        std::unique_ptr<JPH::Factory> m_factory;
        std::unique_ptr<JPH::TempAllocatorImpl> m_tempAllocator;
        std::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem;
        BPLayerInterfaceImpl m_broadPhaseLayerInterface;
        ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;
        ObjectLayerPairFilterImpl m_objectLayerPairFilter;
        std::unique_ptr<JPH::PhysicsSystem> m_physicsSystem;
        BodyActivationListener m_bodyActivationListener;
        ContactListener m_contactListener;
        std::vector<JPH::BodyID> m_destroyList;
        std::queue<std::function<void()>> m_pendingEvents;
        std::unordered_map<entt::entity, JPH::BodyID> m_entityToBodyMap;
        std::unordered_map<JPH::BodyID, entt::entity, BodyIDHash> m_bodyToEntityMap;
        std::unordered_map<entt::entity, std::unordered_set<entt::entity>> m_collidingEntitiesMap;
    };
}

