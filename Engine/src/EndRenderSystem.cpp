#include "CoreSystems/EndRenderSystem.h"
#include "CtxRef.h"
#include "events/BasicEvents.h"
#include "Renderer.h"


void imp::EndRenderSystem::OnToggleGuiEvent()
{
    m_guiEnabled = !m_guiEnabled;
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
    if (m_guiEnabled)
        renderer.endDrawGui();
#endif

    renderer.endFrame();
}


