#pragma once


struct AttackComponent
{
    bool canAttack{ true };
    float attackCooldown{ 0.2f }; //Seconds
    float attackCooldownTimer{ 0.0f };
};