#pragma once
#include "Serialize.h"
#include "CameraControllerSystem.h"
#include "Components/AttackComponent.h"
#include "Components/HealthComponent.h"
#include "Components/GameTagComponents.h"
#include "Systems/MapSystem.h"
#include "Systems/AISystem.h"
#include "Components/TargetComponent.h"
#include "Components/SpeedComponent.h"
#include "Systems/AimingSystem.h"
#include "Components/CameraFollowComponent.h"
#include "Components/DamageComponent.h"
#include "Components/MoveDirectionComponent.h"
#include "Systems/EnemySystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/PlayerControllerSystem.h"
#include "Systems/ProjectileSystem.h"
#include "Systems/ShootingSystem.h"
REGISTER_SYSTEM(CameraControllerSystem)
REGISTER_SYSTEM(MapSystem)
REGISTER_SYSTEM(AISystem)
REGISTER_SYSTEM(AimingSystem)
REGISTER_SYSTEM(PlayerControllerSystem)
REGISTER_SYSTEM(MovementSystem)
REGISTER_SYSTEM(ShootingSystem)
REGISTER_SYSTEM(ProjectileSystem)
REGISTER_SYSTEM(EnemySystem)
REGISTER_AND_SERIALIZE(HealthComponent)
REGISTER_COMPONENT_CUSTOM_NAME(PlayerTag,"Player")
EMPTY_SERIALIZE(PlayerTag)
REGISTER_COMPONENT_CUSTOM_NAME(EnemyTag,"Enemy")
EMPTY_SERIALIZE(EnemyTag)
REGISTER_COMPONENT_CUSTOM_NAME(ProjectileTag,"Projectile")
REGISTER_COMPONENT_CUSTOM_NAME(AimTarget,"AimTarget")
EMPTY_SERIALIZE(ProjectileTag)
REGISTER_AND_SERIALIZE(TargetComponent)
REGISTER_AND_SERIALIZE(SpeedComponent)
REGISTER_COMPONENT_CUSTOM_NAME(AimTarget, "AimTarget")
REGISTER_AND_SERIALIZE(CameraFollowComponent)
REGISTER_AND_SERIALIZE(MoveDirectionComponent)
REGISTER_AND_SERIALIZE(DamageComponent)
REGISTER_AND_SERIALIZE(AttackComponent)

