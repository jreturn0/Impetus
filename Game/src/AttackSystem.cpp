#include "systems/AttackSystem.h"
#include "Components/AttackComponent.h"

void AttackSystem::update(entt::registry& registry, const float deltaTime)
{

    registry.group<AttackComponent>().each([deltaTime](auto entity, AttackComponent& attackComp) {
        if (!attackComp.canAttack) {
            attackComp.attackCooldownTimer -= deltaTime;
            if (attackComp.attackCooldownTimer <= 0.f) {
                attackComp.canAttack = true;
                attackComp.attackCooldownTimer = attackComp.attackCooldown;
            }
        }
        });
}
