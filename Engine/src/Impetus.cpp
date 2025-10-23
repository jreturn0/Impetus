#include "Impetus.h"

#ifdef _DEBUG
#include <imgui_impl_glfw.h>
#endif
#include "CtxRef.h"
#include "FSNavigator.h"
#include "Physics/Physics.h"
#include "SceneArchive.h"
#include "window/Window.h"
using namespace entt::literals;
#include "events/BasicEvents.h"
#include <iostream>
#include <AnsiCodes.h>
#include <CreateRenderer.h>
#include <GLFW/glfw3.h>
#include <fstream>

namespace {
    void ErrorCallback(int error, const char* description)
    {
        //std::string errorString = "Error: " + std::to_string(error) + " " + description;
        Debug::Error("GLFW: {} \n\t{}", error, description);
    }

    bool Init()
    {


        glfwSetErrorCallback(ErrorCallback);

        if (!glfwInit()) {

            exit(EXIT_FAILURE);
        }
        glfwSetErrorCallback(ErrorCallback);
        return true;
    }



    void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        reinterpret_cast<imp::Impetus*>(glfwGetWindowUserPointer(window))->getRenderer().windowResized();
#ifdef _DEBUG

#endif

    }

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        reinterpret_cast<imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<KeyEvent>({ static_cast<Input::Key>(key),scancode,static_cast<Input::Action>(action),static_cast<Input::Mods>(mods) });
#ifdef _DEBUG
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif
        //Imp::Debug::Info("Key Event: {}, {}", key, action);
    }
    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        reinterpret_cast<imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<MouseButtonEvent>({ static_cast<Input::MouseButton>(button),static_cast<Input::Action>(action),static_cast<Input::Mods>(mods) });
#ifdef _DEBUG
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#endif
    }
    
    void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
    {
        reinterpret_cast<imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<MouseMoveEvent>({ xpos,ypos });
#ifdef _DEBUG
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
#endif
    }

    void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        reinterpret_cast<imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<MouseScrollEvent>({ xoffset,yoffset });
#ifdef _DEBUG
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
#endif
    }

    void MouseEnterCallback(GLFWwindow* window, int entered)
    {

        reinterpret_cast<imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<MouseEnterEvent>(MouseEnterEvent{ static_cast<bool>(entered) });
#ifdef _DEBUG
        ImGui_ImplGlfw_CursorEnterCallback(window, entered);
#endif
    }

    void WindowFocusCallback(GLFWwindow* window, int focused)
    {
        reinterpret_cast<imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<WindowFocusEvent>(WindowFocusEvent{ static_cast<bool>(focused) });
#ifdef _DEBUG
        ImGui_ImplGlfw_WindowFocusCallback(window, focused);
#endif
    }
}

void imp::Impetus::registerCallbacks() const
{
    glfwSetFramebufferSizeCallback(m_window->getGLFW(), FramebufferResizeCallback);
    glfwSetKeyCallback(m_window->getGLFW(), KeyCallback);
    glfwSetMouseButtonCallback(m_window->getGLFW(), MouseButtonCallback);
    glfwSetCursorPosCallback(m_window->getGLFW(), MouseMoveCallback);
    glfwSetScrollCallback(m_window->getGLFW(), MouseScrollCallback);
    glfwSetCursorEnterCallback(m_window->getGLFW(), MouseEnterCallback);
    glfwSetWindowFocusCallback(m_window->getGLFW(), WindowFocusCallback);
}

void imp::Impetus::registerEvents()
{

    m_dispatcher->sink<ShutdownEvent>().connect<&Impetus::shutdown>(*this);
    m_dispatcher->sink<SystemStatusEvent>().connect<&SystemManager::onSystemStatusEvent>(m_systemManager);
    m_dispatcher->sink<LoadSceneEvent>().connect<&Impetus::onLoadSceneEvent>(*this);
    m_dispatcher->sink<SaveSceneAsEvent>().connect<&Impetus::onSaveSceneEvent>(*this);
    m_dispatcher->sink<QuickSaveSceneEvent>().connect<&Impetus::onQuickSaveSceneEvent>(*this);
}




void imp::Impetus::registerContextVariables()
{
    m_registry->ctx().insert_or_assign<CtxRef<entt::dispatcher>>(*m_dispatcher);
    m_registry->ctx().insert_or_assign<CtxRef<ResourceManager>>(*m_resourceManager);
    m_registry->ctx().insert_or_assign<CtxRef<gfx::Renderer>>(*m_renderer);
    m_registry->ctx().insert_or_assign<CtxRef<EngineStats>>(*m_engineStats);
    m_registry->ctx().insert_or_assign<CtxRef<phys::Physics>>(*m_physicsEngine);
    m_registry->ctx().insert_or_assign<CtxRef<Clock>>(*time);
    m_registry->ctx().insert_or_assign<CtxRef<utl::ThreadPool>>(*m_threadPool);

}

void imp::Impetus::cleanContextVariables()
{
    if (!m_registry->ctx().erase<CtxRef<entt::dispatcher>>()) {
        Debug::Error("Failed to erase dispatcher");
    }
    if (!m_registry->ctx().erase<CtxRef<ResourceManager>>()) {
        Debug::Error("Failed to erase ResourceManager");
    }

    if (!m_registry->ctx().erase<CtxRef<gfx::Renderer>>()) {
        Debug::Error("Failed to erase Renderer");
    }

    if (!m_registry->ctx().erase<CtxRef<EngineStats>>()) {
        Debug::Error("Failed to erase EngineStats");
    }
}

void imp::Impetus::loadScene(const FSNavigator& fileHelp)
{
    auto selectedFile = fileHelp.getSelectedFile();
    if (selectedFile.has_value()) {
        std::string path = selectedFile.value().string();
        Debug::Info("Loading: {}", path);
        m_registry->clear();
        m_registry = std::make_unique<entt::registry>();
        m_dispatcher->clear();
        m_dispatcher = std::make_unique<entt::dispatcher>();
        m_physicsEngine->getContactListener().updateDispatcher(*m_dispatcher);
        postLoadScene();
        std::ifstream in(path);
        if (in.is_open()) {
            try {
                prepareLoadScene();
                SceneArchive::Load(*m_registry, *m_systemManager, in);
                postLoadScene();
            }
            catch (const std::exception& e) {
                Debug::Error("Error during deserialization: {}", e.what());
            }
        }
        else {
            Debug::Error("Failed to open file: {}", path);
        }
    }
    else {
        Debug::Error("No file selected");
    }
    m_systemManager->initializeSystems(*m_registry);
}

void imp::Impetus::saveScene(const std::string& filename)
{

    Debug::Info("Saving scene to: {}", filename);
    std::ofstream out(filename, std::ios::out | std::ios::trunc);
    if (out.is_open()) {

        SceneArchive::Save(*m_registry, *m_systemManager, out);
    }
    else {
        Debug::Error("Failed to open file: {}", filename);
    }
}

void imp::Impetus::onLoadSceneEvent(LoadSceneEvent event)
{
    m_pendingEvents.emplace([this, event]() {loadScene(event.path); });
    const auto& filename = event.path.getSelectedFile().value().string();
    const auto fullPath = ( filename + (filename.find(".json") == std::string::npos ? ".json" : ""));
    m_currentScenePath = fullPath;
}
void imp::Impetus::onSaveSceneEvent(SaveSceneAsEvent event)
{
    const auto fullPath = (event.path.getCurrentPath().string() + "/" + event.filename + (event.filename.find(".json") == std::string::npos ? ".json" : ""));
    m_currentScenePath = fullPath;
    saveScene(m_currentScenePath);
}

void imp::Impetus::onQuickSaveSceneEvent(QuickSaveSceneEvent event)
{
    if (!std::filesystem::exists(m_currentScenePath)) {
        if (m_currentScenePath.empty()) {
            Debug::Error("No scene path set");
            return;
        }
        Debug::Error("current scene path does not exist: {}", m_currentScenePath);
        return;
    }
    saveScene(m_currentScenePath);
}

void imp::Impetus::prepareLoadScene()
{
    m_systemManager->clearAndCleanup(*m_registry);
    cleanContextVariables();
}

void imp::Impetus::postLoadScene()
{
    registerCallbacks();
    registerContextVariables();
    registerEvents();
}


imp::Impetus::Impetus(const char* title, int fpsCap) :
    m_glfwInitFlag(Init()),
    m_registry(std::make_unique<entt::registry>()), m_systemManager(std::make_unique<SystemManager>()),
    time(std::make_unique<Clock>(glfwGetTime(), fpsCap, 60)),
    m_resourceManager(std::make_unique<ResourceManager>()),
    m_running(false),
    m_renderer(gfx::CreateRenderer(title)), m_window(m_renderer->getSharedWindow()),
    m_engineStats(std::make_unique<EngineStats>()), m_dispatcher(std::make_unique<entt::dispatcher>()),
    m_physicsEngine(std::make_unique<phys::Physics>(*m_dispatcher)),
    m_threadPool(std::make_unique<utl::ThreadPool>(std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1))
{
    Debug::Init();
    Debug::Info("Impetus Created");
}

imp::Impetus::~Impetus()
{

    Debug::Info("Impetus Destroyed");
}




void imp::Impetus::init()
{

    glfwSetWindowUserPointer(m_window->getGLFW(), this);
    Debug::Info("Initializing");
    registerCallbacks();

    registerContextVariables();
    registerEvents();


    m_systemManager->initializeSystems(*m_registry);

}

void imp::Impetus::init(const std::string& sceneFilename, const std::string& dir)
{
    glfwSetWindowUserPointer(m_window->getGLFW(), this);
    Debug::Info("Initializing");
    FSNavigator path;
    if (!dir.empty() && !path.containsFolder(dir)) {
        Debug::Error("Directory not found: {}", dir);
        return;
    }
    if (!path.containsFile(sceneFilename)) {
        Debug::Error("Scene file not found: {}", sceneFilename);
        return;
    }
    path.setSelectedFile(sceneFilename);
    m_currentScenePath = dir + sceneFilename;
    loadScene(path);

}

void imp::Impetus::run()
{
    using namespace utl::hashLiterals;
    m_running = true;
    Debug::Info("Running");


    while (m_running && !glfwWindowShouldClose(m_window->getGLFW())) {
        ENGINESTATS_SCOPED_TIMER(*m_engineStats, "Frame");
        glfwPollEvents();
        time->update();


        {
            ENGINESTATS_SCOPED_TIMER(*m_engineStats, "dispatcher");
            m_dispatcher->update();
        }
        {
            ENGINESTATS_SCOPED_TIMER(*m_engineStats, "deferredEvents");
            while (!m_pendingEvents.empty()) {
                m_pendingEvents.front()();
                m_pendingEvents.pop();
            }
        }
        {
            ENGINESTATS_SCOPED_TIMER(*m_engineStats, "systemsTotal");
            m_systemManager->updateSystems(*m_registry, *time, *m_engineStats);
        }
    }
    Debug::Info("Shutting down");
    m_systemManager->cleanupSystems(*m_registry);



}
void imp::Impetus::shutdown()
{
    m_running = false;
}