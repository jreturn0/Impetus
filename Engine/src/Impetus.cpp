#include "Impetus.h"

#ifdef _DEBUG
#include <imgui_impl_glfw.h>
#endif
#include "CtxRef.h"
#include "CtxRef.h"
#include "FileDirectoryHelper.h"
#include "Physics/Physics.h"
#include "SceneArchive.h"
#include "window/Window.h"
using namespace entt::literals;
#include <BasicEvents.h>
#include <iostream>
#include <AnsiCodes.h>
#include <CtxRef.h>
#include <CreateRenderer.h>
#include <GLFW/glfw3.h>
#include <utils/shader/ShaderCompiler.h>
#include <fstream>


void ErrorCallback(int error, const char* description)
{
	//std::string errorString = "Error: " + std::to_string(error) + " " + description;
	Debug::Error("Error: {} \n\t{}", error, description);
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

void Imp::Impetus::shutdown()
{
	running = false;
}

void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	reinterpret_cast<Imp::Impetus*>(glfwGetWindowUserPointer(window))->getRenderer().windowResized();
#ifdef _DEBUG

#endif

}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	reinterpret_cast<Imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<KeyEvent>({ static_cast<Input::Key>(key),scancode,static_cast<Input::Action>(action),static_cast<Input::Mods>(mods) });
#ifdef _DEBUG
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif
	//Imp::Debug::Info("Key Event: {}, {}", key, action);
}
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	reinterpret_cast<Imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<MouseButtonEvent>({ static_cast<Input::MouseButton>(button),static_cast<Input::Action>(action),static_cast<Input::Mods>(mods) });
#ifdef _DEBUG
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#endif
}
void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	reinterpret_cast<Imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<MouseMoveEvent>({ xpos,ypos });
#ifdef _DEBUG
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
#endif
}

void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	reinterpret_cast<Imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<MouseScrollEvent>({ xoffset,yoffset });
#ifdef _DEBUG
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
#endif
}

void MouseEnterCallback(GLFWwindow* window, int entered)
{

	reinterpret_cast<Imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<MouseEnterEvent>(MouseEnterEvent{ static_cast<bool>(entered) });
#ifdef _DEBUG
	ImGui_ImplGlfw_CursorEnterCallback(window, entered);
#endif
}

void WindowFocusCallback(GLFWwindow* window, int focused)
{
	reinterpret_cast<Imp::Impetus*>(glfwGetWindowUserPointer(window))->getRegistry().ctx().get<CtxRef<entt::dispatcher>>().get().trigger<WindowFocusEvent>(WindowFocusEvent{ static_cast<bool>(focused) });
#ifdef _DEBUG
	ImGui_ImplGlfw_WindowFocusCallback(window, focused);
#endif
}
void Imp::Impetus::registerCallbacks() const
{
	glfwSetFramebufferSizeCallback(*window, FramebufferResizeCallback);
	glfwSetKeyCallback(*window, KeyCallback);
	glfwSetMouseButtonCallback(*window, MouseButtonCallback);
	glfwSetCursorPosCallback(*window, MouseMoveCallback);
	glfwSetScrollCallback(*window, MouseScrollCallback);
	glfwSetCursorEnterCallback(*window, MouseEnterCallback);
	glfwSetWindowFocusCallback(*window, WindowFocusCallback);
}

void Imp::Impetus::registerEvents()
{

	dispatcher->sink<ShutdownEvent>().connect<&Impetus::shutdown>(*this);
	dispatcher->sink<SystemStatusEvent>().connect<&SystemManager::onSystemStatusEvent>(systems);
	dispatcher->sink<LoadSceneEvent>().connect<&Impetus::onLoadSceneEvent>(*this);
	dispatcher->sink<SaveSceneAsEvent>().connect<&Impetus::onSaveSceneEvent>(*this);
	dispatcher->sink<QuickSaveSceneEvent>().connect<&Impetus::onQuickSaveSceneEvent>(*this);
}




void Imp::Impetus::registerContextVariables()
{
	registry->ctx().insert_or_assign<CtxRef<entt::dispatcher>>(*dispatcher);
	registry->ctx().insert_or_assign<CtxRef<ResourceManager>>(*resources);
	registry->ctx().insert_or_assign<CtxRef<Render::Renderer>>(*renderer);
	registry->ctx().insert_or_assign<CtxRef<EngineStats>>(*stats);
	registry->ctx().insert_or_assign<CtxRef<Phys::Physics>>(*physics);
	registry->ctx().insert_or_assign<CtxRef<ThreadPool>>(threadPool);
	registry->ctx().insert_or_assign<CtxRef<Clock>>(*time);

}

void Imp::Impetus::cleanContextVariables()
{
	if (!registry->ctx().erase<CtxRef<entt::dispatcher>>()) {
		Debug::Error("Failed to erase dispatcher");
	}
	if (!registry->ctx().erase<CtxRef<ResourceManager>>()) {
		Debug::Error("Failed to erase ResourceManager");
	}

	if (!registry->ctx().erase<CtxRef<Render::Renderer>>()) {
		Debug::Error("Failed to erase Renderer");
	}

	if (!registry->ctx().erase<CtxRef<EngineStats>>()) {
		Debug::Error("Failed to erase EngineStats");
	}
}

void Imp::Impetus::loadScene(const FileDirectoryHelper& fileHelp)
{
	auto selectedFile = fileHelp.getSelectedFile();
	if (selectedFile.has_value()) {
		std::string path = selectedFile.value().string();// fileHelp.getCurrentPath().string() + "\\" + selectedFile.value().string();
		Debug::Info("Loading: {}", path);
		registry->clear();
		registry = std::make_unique<entt::registry>();
		dispatcher->clear();
		dispatcher = std::make_unique<entt::dispatcher>();
		physics->getContactListener().updateDispatcher(*dispatcher);
		postLoadScene();

		std::ifstream in(path);
		if (in.is_open()) {
			try {
				// Directly pass the stream to SceneArchive::Load
				prepareLoadScene();
				SceneArchive::Load(*registry, *systems, in);
				postLoadScene();
			} catch (const std::exception& e) {
				Debug::Error("Error during deserialization: {}", e.what());
			}
		} else {
			Debug::Error("Failed to open file: {}", path);
		}
	} else {
		Debug::Error("No file selected");
	}
	systems->initializeSystems(*registry);
}

void Imp::Impetus::saveScene(const std::string& filename)
{
	//	auto filename = path.getSelectedFile().value().string();
		//auto fullPath = (path.getCurrentPath().string() + "/" + filename + (filename.find(".json") != std::string::npos ? ".json" : ""));
	Debug::Info("Saving scene to: {}", filename);
	std::ofstream out(filename, std::ios::out | std::ios::trunc);
	if (out.is_open()) {

		SceneArchive::Save(*registry, *systems, out);
	} else {
		Debug::Error("Failed to open file: {}", filename);
	}
}

void Imp::Impetus::onLoadSceneEvent(LoadSceneEvent event)
{
	//prepareLoadScene();
	deferredEvents.emplace([this, event]() {loadScene(event.path); });
	//loadScene(event.path);
	const auto& filename = event.path.getSelectedFile().value().string();
	const auto fullPath = (event.path.getCurrentPath().string() + "/" + filename + (filename.find(".json") != std::string::npos ? ".json" : ""));
	scenePath = fullPath;
}
void Imp::Impetus::onSaveSceneEvent(SaveSceneAsEvent event)
{

	const auto fullPath = (event.path.getCurrentPath().string() + "/" + event.filename + (event.filename.find(".json") != std::string::npos ? ".json" : ""));
	scenePath = fullPath;
	saveScene(scenePath);
}

void Imp::Impetus::onQuickSaveSceneEvent(QuickSaveSceneEvent event)
{
	if (scenePath.empty()) {
		Debug::Error("No scene path set");
		return;
	}
	saveScene(scenePath);
}

void Imp::Impetus::prepareLoadScene()
{
	//registry.clear();
	systems->clearAndCleanup(*registry);
	//std::cout << dispatcher.size() << " 1 \n";
	//dispatcher.clear();
	cleanContextVariables();

}

void Imp::Impetus::postLoadScene()
{
	registerCallbacks();
	registerContextVariables();
	//std::cout << dispatcher.size() << "2\n";
	registerEvents();
	//std::cout << dispatcher.size() << "3\n";

	//systems->initializeSystems(*registry);
}


Imp::Impetus::Impetus(uint32_t width, uint32_t height, const char* title, int fpsCap) :
	glfwInitFlag(Init()),
	registry(std::make_unique<entt::registry>()), systems(std::make_unique<SystemManager>()),
	time(std::make_unique<Clock>(fpsCap, 60)),
	resources(std::make_unique<ResourceManager>()),
	running(false),
	renderer(Render::CreateRenderer(width, height, title, "triangle_mesh")), window(renderer->getSharedWindow()),
	stats(std::make_unique<EngineStats>()), dispatcher(std::make_unique<entt::dispatcher>()),
	physics(std::make_unique<Phys::Physics>(*dispatcher))
{
	Debug::Init();
	Debug::Info("Impetus Created");
}

Imp::Impetus::~Impetus()
{

	Debug::Info("Impetus Destroyed");
}




void Imp::Impetus::init()
{

	glfwSetWindowUserPointer(*window, this);
	Debug::Info("Initializing");
	registerCallbacks();
	
	registerContextVariables();
	registerEvents();



	//systems.reload();
	systems->initializeSystems(*registry);

}

void Imp::Impetus::init( const std::string& sceneFilename, const std::string& dir )
{
	glfwSetWindowUserPointer(*window, this);
	Debug::Info("Initializing");
	FileDirectoryHelper path;
	if (!dir.empty() && !path.checkForDirectory(dir)) {
		Debug::Error("Directory not found: {}", dir);
		return;
	}
	if (!path.checkForLocalFile(sceneFilename)) {
		Debug::Error("Scene file not found: {}", sceneFilename);
		return;
	}
	path.setSelectedLocalFile(sceneFilename);
	scenePath = dir + sceneFilename;
	loadScene(path);

}

void Imp::Impetus::run()
{

	running = true;
	Debug::Info("Running");
	RECORD_STATS_VARIABLES
		RECORD_STATS_VARIABLES_N(1)

		while (running && !glfwWindowShouldClose(*window)) {
			//auto start = std::chrono::high_resolution_clock::now();
			RECORD_STATS_START_N(1)
				glfwPollEvents();

			time->update();

			RECORD_STATS_START

				dispatcher->update();
			while (!deferredEvents.empty()) {
				deferredEvents.front()();
				deferredEvents.pop();
			}
			RECORD_STATS_END(stats, eventDispatchTime);

			RECORD_STATS_START
				systems->updateSystems(*registry, *time, *stats);
			RECORD_STATS_END(stats, updateTime)
				RECORD_STATS_END_N(stats, frameTime, 1)

		}
	Debug::Info("Shutting down");
	systems->cleanupSystems(*registry);



}
