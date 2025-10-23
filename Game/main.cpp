#include "ComponentInfoRegistry.h"
#include "Components/AimTargetComponent.h"
#include "Components/AttackComponent.h"
#include "Components/CameraControllerComponent.h"
#include "Components/CameraFollowComponent.h"
#include "Components/DamageComponent.h"
#include "Components/GameTagComponents.h"
#include "Components/HealthComponent.h"
#include "Components/MoveDirectionComponent.h"
#include "Components/ProjectileWeaponComponent.h"
#include "Components/RenderSceneDataComponent.h"
#include "Components/SpeedComponent.h"
#include "Components/TargetComponent.h"
#include "CoreSerialize.h"
#include "CoreSystems/CameraSystem.h"
#include "CoreSystems/EndRenderSystem.h"
#include "CoreSystems/InputSystem.h"
#include "CoreSystems/PhysicsSystem.h"
#include "CoreSystems/PostPhysicsSystem.h"
#include "CoreSystems/PrePhysicsSystem.h"
#include "CoreSystems/RenderSystem.h"
#include "CoreSystems/ResourceSystem.h"
#include "CoreSystems/TransformSystem.h"
#include "Debug.h"
#ifdef _DEBUG
#include "EditorGuiSystem.h"
#endif
#include "Impetus.h"
#include "Systems/AimingSystem.h"
#include "Systems/AISystem.h"
#include "Systems/CameraControllerSystem.h"
#include "Systems/CameraFollowSystem.h"
#include "Systems/MapSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/PlayerControllerSystem.h"
#include "Systems/ProjectileSystem.h"
#include "Systems/ShootingSystem.h"
#include "Systems/DamageSystem.h"
#include "Systems/EnemySpawnSystem.h"
#include "Components/EnemySpawnPointComponent.h"
#include <Systems/ComputeShaderSystem.h>
#include "Systems/AttackSystem.h"
REGISTER_SYSTEM(CameraControllerSystem)
REGISTER_SYSTEM(MapSystem)
REGISTER_SYSTEM(AISystem)
REGISTER_SYSTEM(AimingSystem)
REGISTER_SYSTEM(PlayerControllerSystem)
REGISTER_SYSTEM(MovementSystem)
REGISTER_SYSTEM(ShootingSystem)
REGISTER_SYSTEM(ProjectileSystem)
REGISTER_SYSTEM(DamageSystem)
REGISTER_SYSTEM(EnemySpawnSystem)
REGISTER_SYSTEM(CameraFollowSystem)
REGISTER_SYSTEM(AttackSystem)
REGISTER_COMPONENT_CUSTOM_NAME(PlayerTag, "Player")
EMPTY_SERIALIZE(PlayerTag)
REGISTER_COMPONENT_CUSTOM_NAME(EnemyTag, "Enemy")
EMPTY_SERIALIZE(EnemyTag)
REGISTER_COMPONENT_CUSTOM_NAME(ProjectileTag, "Projectile")
EMPTY_SERIALIZE(ProjectileTag)
REGISTER_AND_SERIALIZE(TargetComponent)
REGISTER_AND_SERIALIZE(SpeedComponent)
REGISTER_AND_SERIALIZE(AimTargetComponent)
REGISTER_AND_SERIALIZE(CameraFollowComponent)
REGISTER_AND_SERIALIZE(MoveDirectionComponent)
REGISTER_AND_SERIALIZE(ProjectileWeaponComponent)
REGISTER_AND_SERIALIZE(EnemySpawnPointComponent)
REGISTER_AND_SERIALIZE(RenderSceneDataComponent)
REGISTER_AND_SERIALIZE(DamageComponent)
REGISTER_AND_SERIALIZE(HealthComponent)
REGISTER_AND_SERIALIZE(AttackComponent)
REGISTER_AND_SERIALIZE(CameraControllerComponent)



// Custom ImGui widgets for HealthComponent and DamageComponent
// Only used inside the editor, not sure how else I'd give the editor this info without 
// adding it here
#ifdef _DEBUG
#include <imgui.h>
#include "ComponentEditorRegistry.h"
inline void HealthDamageLayersWidget(HealthDamageFlags& layers) {
    HealthDamageFlags::MaskType mask = layers.getMask();
    ImGui::CheckboxFlags("Player", &mask, HealthDamageLayers::Player1);
    ImGui::CheckboxFlags("Enemy", &mask, HealthDamageLayers::Enemy2);
    ImGui::CheckboxFlags("Environment", &mask, HealthDamageLayers::Environment3);
    ImGui::CheckboxFlags("Layer4", &mask, HealthDamageLayers::Layer4);
    ImGui::CheckboxFlags("Layer5", &mask, HealthDamageLayers::Layer5);
    ImGui::CheckboxFlags("Layer6", &mask, HealthDamageLayers::Layer6);
    ImGui::CheckboxFlags("Layer7", &mask, HealthDamageLayers::Layer7);
    ImGui::CheckboxFlags("Layer8", &mask, HealthDamageLayers::Layer8);
}

inline void DamageWidgetInfo(entt::registry& registry, entt::entity entity)
{
    auto& damage = registry.get<DamageComponent>(entity);
    ImGui::InputInt("Damage", &damage.damage);
    HealthDamageLayersWidget(damage.layers);
}
inline void HealthWidgetInfo(entt::registry& registry, entt::entity entity)
{
    auto& health = registry.get<HealthComponent>(entity);
    ImGui::InputInt("Health", &health.health);
    ImGui::InputInt("Max Health", &health.maxHealth);
    HealthDamageLayersWidget(health.layers);
}
REGISTER_CUSTOM_COMPONENT_WIDGET(DamageComponent, DamageWidgetInfo)
REGISTER_CUSTOM_COMPONENT_WIDGET(HealthComponent, HealthWidgetInfo)
#endif //_debug








int main()
{
    constexpr auto logMask = Debug::g_allLogTypes ^ Debug::Log::Type::Trace ^ Debug::Log::Type::Info;
    Debug::Init();
    auto engine = imp::Impetus("Impetus", 60);
    engine.emplaceAndRegisterSystem<imp::InputSystem>();
    engine.emplaceAndRegisterSystem<imp::ResourceSystem>();
    engine.emplaceAndRegisterSystem<imp::TransformSystem>();
    engine.emplaceAndRegisterSystem<imp::CameraSystem>();
    engine.emplaceAndRegisterSystem<CameraControllerSystem>();
    engine.emplaceAndRegisterSystem<ComputeShaderSystem>();
    engine.emplaceAndRegisterSystem<imp::RenderSystem>();
#ifdef _DEBUG
    engine.emplaceAndRegisterSystem<EditorGuiSystem>();
#endif // _DEBUG
    engine.emplaceAndRegisterSystem<imp::EndRenderSystem>();
    engine.emplaceAndRegisterSystem<imp::PrePhysicsSystem>(imp::UpdateType::PreFixed);
    engine.emplaceAndRegisterSystem<imp::PhysicsSystem>(imp::UpdateType::Fixed);
    engine.emplaceAndRegisterSystem<imp::PostPhysicsSystem>(imp::UpdateType::PostFixed);
    engine.init();
    engine.run();
}
