#include "CoreSystems/EndRenderSystem.h"

#include "BasicEvents.h"
#include "CtxRef.h"
#include "Renderer.h"


void imp::EndRenderSystem::OnToggleGuiEvent()
{
	gui = !gui;
}

void imp::EndRenderSystem::initialize(entt::registry& registry)
{
	registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<ToggleGuiEvent>().connect<&EndRenderSystem::OnToggleGuiEvent>(this);

	System::initialize(registry);
}

void imp::EndRenderSystem::update(entt::registry& registry, const float deltaTime)
{
	auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
#ifdef _DEBUG
	if (gui)
		renderer.endDrawGui();
#endif

	renderer.endFrame();
}

void imp::EndRenderSystem::cleanup(entt::registry& registry) { System::cleanup(registry); }
