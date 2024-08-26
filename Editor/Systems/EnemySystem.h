#pragma once


#include <System.h>

class EnemySystem : public System
{
private:
	size_t enemyCount = 0;
	size_t targetEnemyCount = 10;
public:

	void OnEnemyDeath(entt::registry& registry, entt::entity entity);

	void update(entt::registry& registry, const float deltaTime) override;
};
