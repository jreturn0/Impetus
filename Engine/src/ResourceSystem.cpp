#include "CoreSystems/ResourceSystem.h"
#include <BasicEvents.h>
#include <ResourceTypes/TestResource.h>
#include <ResourceManager.h>

#include "CtxRef.h"


void imp::ResourceSystem::onTestResourceConstruct(entt::registry& registry, entt::entity entity)
{
	////auto&& comp = registry.get<TestComponent>(entity);

	//if (auto&& pair = resourceManager->loadAndGet<TestResource>(comp.resource.path); pair.first) {
	//	auto&& handle = pair.first;
	//	comp.resource.handle = handle;
	//}
	
};

void imp::ResourceSystem::initialize(entt::registry& registry)
{
	/*resourceManager = &registry.ctx().get<CtxRef<ResourceManager>>().get();
	registry.on_construct <TestComponent>().connect<&ResourceSystem::onTestResourceConstruct>(*this);
	*/
	registry.ctx().get<CtxRef<entt::dispatcher>>().get().enqueue<SystemStatusEvent>("Imp::ResourceSystem", false);
}

void imp::ResourceSystem::update(entt::registry & registry, const float deltaTime)
{
//	Imp::Debug::Info("ResourceSystem Update");

}
