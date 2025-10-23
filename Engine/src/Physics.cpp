#include "ConfigSystem.h"
#include "Physics/Physics.h"
#include <cstdarg>
#include <iostream>
#include <thread>

namespace {
    auto cfg_physicsMaxBodies = utl::ConfigValueRef("physics.max_bodies", 1024);
    auto cfg_physicsNumBodyMutexes = utl::ConfigValueRef("physics.num_body_mutexes", 4);
    auto cfg_physicsMaxBodyPairs = utl::ConfigValueRef("physics.max_body_pairs", 1024);
    auto cfg_physicsMaxContactConstraints = utl::ConfigValueRef("physics.max_contact_constraints", 1024);
    auto cfg_physicsCollisionSteps = utl::ConfigValueRef("physics.collision_steps", 1);
    auto cfg_physicsMaxTrackedContacts = utl::ConfigValueRef("physics.max_tracked_contacts", 4);
    auto cfg_physicsMemSize = utl::ConfigValueRef("physics.temp_mem_size", 10 * 1024 * 1024); //10 MB
    auto cfg_physicsGravityX = utl::ConfigValueRef("physics.gravity_x", 0.);
    auto cfg_physicsGravityY = utl::ConfigValueRef("physics.gravity_y", -9.82); // Earth style
    auto cfg_physicsGravityZ = utl::ConfigValueRef("physics.gravity_z", 0.); 
}
imp::phys::Physics::Physics(entt::dispatcher& eventDispatcher) : m_contactListener(eventDispatcher, m_bodyToEntityMap)
{
    /// This is just boilerplate code to get Jolt Physics up and running 
    /// It's not optimized or anything, just the minimum required to get it working
    /// Ideally the implementation would be more thorough but this is what I had time for
    /// The comments bellow are literally from the Jolt Physics examples

    m_destroyList.reserve(cfg_physicsMaxBodies.getCopy());

    // Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
    // This needs to be done before any other Jolt function is called.
    JPH::RegisterDefaultAllocator();

    // Install trace and assert callbacks
    JPH::Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl);

    // Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
    // It is not directly used in this example but still required.
    JPH::Factory::sInstance = new JPH::Factory();

    // Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
    // If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
    // If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
    JPH::RegisterTypes();

    // We need a temp allocator for temporary allocations during the physics update. We're
    // pre-allocating 10 MB to avoid having to do allocations during the physics update.
    // B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
    // If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
    // malloc / free.
    m_tempAllocator = std::make_unique< JPH::TempAllocatorImpl>(cfg_physicsMemSize.getCopy());

    // We need a job system that will execute physics jobs on multiple threads. Typically
    // you would implement the JobSystem interface yourself and let Jolt Physics run on top
    // of your own job scheduler. JobSystemThreadPool is an example implementation.
    m_jobSystem = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
    m_physicsSystem = std::make_unique<JPH::PhysicsSystem>();
    m_physicsSystem->Init(cfg_physicsMaxBodies.getCopy(), cfg_physicsNumBodyMutexes.getCopy(), cfg_physicsMaxBodyPairs.getCopy(), cfg_physicsMaxContactConstraints.getCopy(), m_broadPhaseLayerInterface, m_objectVsBroadPhaseLayerFilter, m_objectLayerPairFilter);
    m_physicsSystem->SetBodyActivationListener(&m_bodyActivationListener);
    m_physicsSystem->SetContactListener(&m_contactListener);
    m_physicsSystem->SetGravity({ static_cast<float>(cfg_physicsGravityX.getCopy()),static_cast<float>(cfg_physicsGravityY.getCopy()),static_cast<float>(cfg_physicsGravityZ.getCopy())});
}


imp::phys::Physics::~Physics()
{
    JPH::UnregisterTypes();
}

imp::phys::RayHit imp::phys::Physics::getMouseWorldPosition(const glm::vec2& mousePosition, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::vec2& viewport) const
{
    float x = (2.0f * mousePosition.x) / viewport.x - 1.0f;
    float y = (2.0f * mousePosition.y) / viewport.y - 1.0f;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye.z = -1.0f;
    rayEye.w = 0.0f;
    glm::vec3 rayWorld = glm::normalize(glm::vec3(viewMatrix * rayEye));
    glm::vec3 cameraPosition = glm::vec3(viewMatrix[3]);
    return rayCast(cameraPosition, rayWorld, 1000.0f);

}

imp::phys::RayHit imp::phys::Physics::rayCast(const glm::vec3& origin, const glm::vec3& direction, const float distance) const
{
    const JPH::RRayCast ray{ ToJPH(origin), ToJPH(direction) * distance };
    JPH::RayCastResult hit{};
    const bool didHit = m_physicsSystem->GetNarrowPhaseQuery().CastRay(ray, hit);
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
        ToGLM(m_physicsSystem->GetBodyLockInterfaceNoLock().TryGetBody(hit.mBodyID)->GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, point)),
        hit.mFraction,
    };
}

JPH::BodyID imp::phys::Physics::createBody(entt::entity owner, const JPH::Shape* shape, const glm::vec3& position, const glm::quat& rotation, JPH::EMotionType motionType, ObjectLayer layer, JPH::EActivation activation)
{

    JPH::BodyCreationSettings settings(shape, ToJPH(position), ToJPH(rotation), motionType, (layer));
    settings.mAllowDynamicOrKinematic = true;

    auto&& body = m_physicsSystem->GetBodyInterface().CreateAndAddBody(settings, activation);
    if (body.IsInvalid()) {
        Debug::Error("Failed to create body\n");
    }
    m_entityToBodyMap[owner] = body;
    m_bodyToEntityMap[body] = owner;
    return 	body;

}

inline void imp::phys::Physics::destroyBody(entt::entity owner) { destroyBody(m_entityToBodyMap[owner]); }

void imp::phys::Physics::destroyBodies()
{
    if (m_destroyList.empty())
        return;
    for (const auto& body : m_destroyList) {
        m_physicsSystem->GetBodyInterface().IsActive(body);
    }

    m_physicsSystem->GetBodyInterface().RemoveBodies(&*m_destroyList.begin(), m_destroyList.size());
    m_physicsSystem->GetBodyInterface().DestroyBodies(&*m_destroyList.begin(), m_destroyList.size());
    m_destroyList.clear();
}

void imp::phys::Physics::destroyBody(JPH::BodyID body)
{
    m_entityToBodyMap.erase(m_bodyToEntityMap[body]);
    m_bodyToEntityMap.erase(body);
    m_destroyList.push_back(body);    
}

void imp::phys::Physics::update(float deltaTime)
{
    while (!m_pendingEvents.empty()) {
        m_pendingEvents.front()();
        m_pendingEvents.pop();
    }
    destroyBodies();
    m_physicsSystem->Update(deltaTime, cfg_physicsCollisionSteps.getCopy(), &*m_tempAllocator, &*m_jobSystem);
}
