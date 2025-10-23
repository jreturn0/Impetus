#include "CoreSystems/ResourceSystem.h"
#include "CtxRef.h"
#include "events/BasicEvents.h"
#include <ResourceManager.h>
#include <ResourceTypes/TestResource.h>




void imp::ResourceSystem::initialize(entt::registry& registry)
{
	registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemStatusEvent>("Imp::ResourceSystem", false);
}

void imp::ResourceSystem::update(entt::registry & registry, const float deltaTime)
{

}
