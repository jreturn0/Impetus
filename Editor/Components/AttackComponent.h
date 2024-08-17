#pragma once


struct AttackComponent
{
	bool canAttack = true;
	float attackCooldown = 1.0f; // 1 second
	float attackCooldownTimer = 0.0f; 
};