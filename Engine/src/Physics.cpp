#include "Physics/Physics.h"
#include <iostream>
#include <cstdarg>
#include <thread>

namespace {





}
imp::Phys::Physics::Physics(entt::dispatcher& eventDispatcher) : contactListener(eventDispatcher,bodyToEntityMap)
{
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
	tempAllocator = std::make_unique< JPH::TempAllocatorImpl>(10 * 1024 * 1024);

	// We need a job system that will execute physics jobs on multiple threads. Typically
	// you would implement the JobSystem interface yourself and let Jolt Physics run on top
	// of your own job scheduler. JobSystemThreadPool is an example implementation.
	jobSystem = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

	physicsSystem = std::make_unique<JPH::PhysicsSystem>();
	physicsSystem->Init(MAX_BODIES, NUM_BODY_MUTEXES, MAX_BODY_PAIRS, MAX_CONTACT_CONSTRAINTS, broadPhaseLayerInterface, objectVsBroadPhaseLayerFilter, objectLayerPairFilter);


	physicsSystem->SetBodyActivationListener(&bodyActivationListener);


	physicsSystem->SetContactListener(&contactListener);

	physicsSystem->SetGravity({ 0.f,-9.82f,0.f });




}

imp::Phys::Physics::Physics() 
{
}

imp::Phys::Physics::~Physics()
{
	JPH::UnregisterTypes();

}
