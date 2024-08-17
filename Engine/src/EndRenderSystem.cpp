#include "CoreSystems/EndRenderSystem.h"

#include "BasicEvents.h"
#include "CtxRef.h"
#include "Renderer.h"


void Imp::EndRenderSystem::OnToggleGuiEvent()
{
	gui = !gui;
}

void Imp::EndRenderSystem::initialize(entt::registry& registry)
{
	registry.ctx().get<CtxRef<entt::dispatcher>>().get().sink<ToggleGuiEvent>().connect<&EndRenderSystem::OnToggleGuiEvent>(this);

	System::initialize(registry);
}

void Imp::EndRenderSystem::update(entt::registry& registry, const float deltaTime)
{
	auto& renderer = registry.ctx().get<CtxRef<Imp::Render::Renderer>>().get();
#ifdef _DEBUG
	if (gui)
		renderer.endDrawGui();
#endif

	renderer.endFrame();
}

void Imp::EndRenderSystem::cleanup(entt::registry& registry) { System::cleanup(registry); }
