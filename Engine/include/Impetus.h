#pragma once
#include "Clock.h"
#include "FSNavigator.h"
#include "ResourceManager.h"
#include "SystemManager.h"
#include "SystemRegistry.h"
#include "ThreadPool.h"
#include <entt/entity/registry.hpp>
#include <fstream>






struct GLFWwindow;
namespace imp {
    namespace gfx
    {
        class Renderer;
    }
    namespace phys
    {
        class Physics;
    }
    class Window;
    class Impetus
    {
    public:
        Impetus(const char* title, int fpsCap = 0);
        ~Impetus();
        Impetus(const Impetus&) = delete;
        Impetus& operator=(const Impetus&) = delete;
        Impetus(Impetus&&) = delete;
        Impetus&& operator=(Impetus&&) = delete;
        
        // Getters
        inline entt::registry& getRegistry() const { return *m_registry; }
        inline phys::Physics& getPhysics() const { return *m_physicsEngine; }
        inline gfx::Renderer& getRenderer() { return *m_renderer; }

        // Methods

        template<typename T,typename TLoader>
        void registerResourceAndLoader()
        {
            ResourceRegistry::Register<T, TLoader>();
        }
        template<typename T>
        void emplaceAndRegisterSystem(const UpdateType type = UpdateType::Variable) const
        {
            m_systemManager->addSystem(SystemRegistry::RegisterSystem<T>(),true, type);
        }

        void init();
        void init(const std::string& sceneFilename, const std::string& dir = "");
        void run();

    private:
        bool m_glfwInitFlag{ false };
        std::unique_ptr<entt::registry> m_registry{nullptr};
        std::unique_ptr<SystemManager> m_systemManager{ nullptr };
        std::unique_ptr<Clock> time{ nullptr };
        std::unique_ptr<ResourceManager> m_resourceManager{ nullptr };
        std::atomic<bool> m_running{ false };
        std::unique_ptr<gfx::Renderer> m_renderer{ nullptr };
        std::shared_ptr<Window> m_window{ nullptr };
        std::unique_ptr<EngineStats> m_engineStats{ nullptr };
        std::string m_currentScenePath{};
        std::unique_ptr < entt::dispatcher >m_dispatcher{ nullptr };
        std::queue<std::function<void()>> m_pendingEvents{};
        std::unique_ptr<phys::Physics> m_physicsEngine{ nullptr };
        std::unique_ptr<utl::ThreadPool> m_threadPool{ nullptr };


        void shutdown();
        void registerCallbacks() const;
        void registerEvents();
        void registerContextVariables();
        void cleanContextVariables();
        void loadScene(const FSNavigator& fileHelp);
        void saveScene(const std::string& filename);
        void onLoadSceneEvent(LoadSceneEvent event);
        void onSaveSceneEvent(SaveSceneAsEvent event);
        void onQuickSaveSceneEvent(QuickSaveSceneEvent event);
        void prepareLoadScene();
        void postLoadScene();


    };
}
