#include "Systems/ComputeShaderSystem.h"
#include <CtxRef.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <Impetus.h>
#include <Renderer.h>
//#include "gpudata/GPUSceneData.h"
#include <Components/RenderSceneDataComponent.h>
#include "core/ComputeEffect.h"
#include <Components/TransformComponent.h>
#include <Components/CameraComponent.h>
#include <Components/Tags.h>
void ComputeShaderSystem::initialize(entt::registry& registry)
{
    auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();
    renderer.loadComputeShader("sky_gradient");
    renderer.loadComputeShader("sky2");
}

void ComputeShaderSystem::update(entt::registry& registry, const float deltaTime)
{
    auto view = registry.view<imp::TransformComponent, imp::CameraComponent, imp::ActiveCameraTag>();
    if (view.begin() == view.end()) return;
    auto camEntity = *view.begin();
    RenderSceneDataComponent sceneDataComp = registry.get_or_emplace<RenderSceneDataComponent>(camEntity);
    auto& transform = registry.get<imp::TransformComponent>(camEntity);
    auto& renderer = registry.ctx().get<CtxRef<imp::gfx::Renderer>>().get();

    auto& sceneData = renderer.getSceneData();
    sceneData.ambientColor = sceneDataComp.ambientColor;
    sceneData.sunlightColor = sceneDataComp.sunlightColor;
    sceneData.sunlightDirection = sceneDataComp.sunlightDirection;
    auto& effect = renderer.getCurrentComputeEffect();
    auto& pushes = effect.getPushConstants();
    static float time = 0.f;
    time += deltaTime;    
    pushes[0] = sceneDataComp.computePush1;
    pushes[1] = sceneDataComp.computePush2;
    pushes[2].x = transform.position.x;
    pushes[2].y = transform.position.y;
    pushes[2].z = transform.position.z;
    pushes[2].w = time;

    pushes[3].x = transform.rotation.x;
    pushes[3].y = transform.rotation.y;
    pushes[3].z = transform.rotation.z;
    pushes[3].w = transform.rotation.w;
}
