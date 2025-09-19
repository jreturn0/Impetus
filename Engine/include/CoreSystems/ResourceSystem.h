#pragma once
#include "System.h"
namespace imp {
	class ResourceSystem : public System
	{
	private:
		/*std::reference_wrapper<class ResourceManager>*/ class ResourceManager* resourceManager;

		void onTestResourceConstruct(entt::registry& registry, entt::entity entity);

	public:
		void initialize(entt::registry& registry) override;

		// Inherited via System
		void update(entt::registry& registry, const float deltaTime) override;
	};

}