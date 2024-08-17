#pragma once
#include <fstream>
#include <entt/entity/registry.hpp>


#include "SystemManager.h"
#include "SystemRegistry.h"
#include "ResourceManager.h"
#include"Clock.h"
#include "ThreadPool.h"
#include "FileDirectoryHelper.h"


inline static std::ofstream logFile ("log.txt");



struct GLFWwindow;
namespace Imp {
	namespace Render
	{
		class Renderer;
	}
	namespace Phys
	{
		class Physics;
	}
	class Window;
	class Impetus
	{
	private:
		bool glfwInitFlag;
		std::unique_ptr < entt::registry >registry;
		std::unique_ptr < SystemManager >systems;
		std::unique_ptr < Clock >time;
		std::unique_ptr < ResourceManager >resources;
		std::atomic<bool> running;
		std::unique_ptr<Render::Renderer> renderer;
		std::shared_ptr<Window> window;
		std::unique_ptr < EngineStats >stats;
		std::string scenePath;
		std::unique_ptr < entt::dispatcher >dispatcher;
		std::queue<std::function<void()>> deferredEvents;
		std::unique_ptr<Phys::Physics> physics;
		ThreadPool threadPool;
		void shutdown();

		void registerCallbacks() const;
		void registerEvents();
		void registerContextVariables();

		void cleanContextVariables();
		void loadScene(const FileDirectoryHelper& fileHelp);
		void saveScene(const std::string& filename);
		void onLoadSceneEvent(LoadSceneEvent event);
		void onSaveSceneEvent(SaveSceneAsEvent event);

		void onQuickSaveSceneEvent(QuickSaveSceneEvent event);

		void prepareLoadScene();
		void postLoadScene();


	public:

		auto& getRegistry() const { return *registry; }
	
		auto& getPhysics() const { return *physics; }
		Render::Renderer& getRenderer() { return *renderer; }

		Impetus(uint32_t width, uint32_t height, const char* title,int fpsCap=0);
		~Impetus();
		template<typename T,typename TLoader>
		void registerResourceAndLoader()
		{
			ResourceRegistry::Register<T, TLoader>();
		}

		template<typename T>
		void emplaceAndRegisterSystem(const UpdateType type = UpdateType::Variable) const
		{
			SystemRegistry::RegisterSystem<T>();
			std::string name = typeid(T).name();
			name = name.substr(6);
			systems->addSystem(name.c_str(),true, type);
			
		}

		void init();

		void init(const std::string& sceneFilename, const std::string& dir = "");

		void run();


	};
}
