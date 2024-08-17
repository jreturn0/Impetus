#pragma once
#include <entt/entity/fwd.hpp>
namespace Imp {
	class SystemManager;

	class SceneArchive
	{
		static bool EntityHasComponent(entt::entity& entity, entt::registry& registry, entt::id_type component);

	public:
		static void Save(entt::registry& registry, SystemManager& systemManager, std::ostream& output);

		static void Load(entt::registry& registry, SystemManager& systemManager, std::istream& input);
	};

}
