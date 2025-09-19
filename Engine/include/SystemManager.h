#pragma once
#include "System.h"
#include <BasicEvents.h>
#include <map>
#include <cereal/cereal.hpp>
#include <cereal/types/unordered_map.hpp>

#include "Debug.h"
#include "EngineStats.h"
#include "UpdateType.h"

namespace imp {










	class Clock;


	class SystemManager
	{
	public:

	private:
		struct Entry
		{
			std::unique_ptr<System> system;
			bool status{ false };
			std::string name;

			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(status, name);
			}
		};
		struct Meta
		{
			size_t index;
			UpdateType type;
			template <class Archive>
			void serialize(Archive& ar)
			{
				ar(index, type);
			}
		};

		std::vector<Entry> systems;
		std::vector<Entry> preFixedSystems;
		std::vector<Entry> fixedSystems;
		std::vector<Entry> postFixedSystems;
		std::unordered_map<std::string, Meta> systemMetaMap;
		std::queue<std::function<void(entt::registry&)>> systemEvents;
		bool reorderDispatchPending = true;
		//std::map<size_t, std::string> systemsOrder;

		std::vector<Entry>& getContainer(const UpdateType type);

	public:

		void OnSystemReorderEvent(SystemReorderEvent& event);
		void OnSystemAddEvent(AddSystemEvent& event);
		void OnSystemRemoveEvent(RemoveSystemEvent& event);

		template <class Archive>
		void serialize(Archive& ar)
		{
			ar(cereal::make_nvp("systems", systems),
			   cereal::make_nvp("preFixedSystems", preFixedSystems),
			   cereal::make_nvp("fixedSystems", fixedSystems),
			   cereal::make_nvp("postFixedSystems", postFixedSystems),
			   cereal::make_nvp("systemMetaMap", systemMetaMap));

			//	reload();
		}
		//template <class Archive>
		//static void load_and_construct(Archive& ar, cereal::construct<SystemManager>& construct)
		//{
		//	Debug::Info("Loading SystemManager-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
		//	//Debug::Out("Loading SystemManager");
		//	construct(); // Default constructor
		//	ar(cereal::make_nvp("systems", construct->systems),
		//	   cereal::make_nvp("preFixedSystems", construct->preFixedSystems),
		//	   cereal::make_nvp("fixedSystems", construct->fixedSystems),
		//	   cereal::make_nvp("postFixedSystems", construct->postFixedSystems),
		//	   cereal::make_nvp("systemMetaMap", construct->systemMetaMap));
		//	construct->reload();
		//}
		void addSystem(const std::string& name, bool enabled = true, UpdateType type = UpdateType::Variable);

		void initializeSystems(entt::registry& registry);

		void updateSystems(entt::registry& registry, Clock& time, EngineStats& stats);

		void cleanupSystems(entt::registry& registry) const;

		void setSystemStatus(const std::string& name, bool status);

		void onSystemStatusEvent(const SystemStatusEvent& statusEvent);

		void clear();
		void clearAndCleanup(entt::registry& registry);

		void reload();
	};



}
