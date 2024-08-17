#include "Debug.h"
#include "Impetus.h"
#include "Serialize_.h"
#include "CameraControllerSystem.h"
#include "CoreSystems/EndRenderSystem.h"
#include "CoreSystems/InputSystem.h"
#include "CoreSystems/RenderSystem.h"
#include "CoreSystems/ResourceSystem.h"



#include "EditorGuiSystem.h"
#include "CoreSystems/CameraSystem.h"
#include "CoreSystems/PhysicsSystem.h"
#include "CoreSystems/PostPhysicsSystem.h"
#include "CoreSystems/PrePhysicsSystem.h"
#include "CoreSystems/TransformSystem.h"

int main()
{
	//try {


		//	std::string str;
		//	std::cin >> str;
		Debug::Init();
	auto engine = Imp::Impetus(1920, 1080, "Impetus");
		//engine.registerResourceAndLoader<MyResource, MyResourceLoader>();
		engine.emplaceAndRegisterSystem<Imp::InputSystem>();
		engine.emplaceAndRegisterSystem<Imp::ResourceSystem>();
		engine.emplaceAndRegisterSystem<Imp::TransformSystem>();
		engine.emplaceAndRegisterSystem<Imp::PrePhysicsSystem>(Imp::UpdateType::PreFixed);
		engine.emplaceAndRegisterSystem<Imp::PhysicsSystem>(Imp::UpdateType::Fixed);
		engine.emplaceAndRegisterSystem<Imp::PostPhysicsSystem>(Imp::UpdateType::PostFixed);
		engine.emplaceAndRegisterSystem<CameraControllerSystem>();
		engine.emplaceAndRegisterSystem<Imp::CameraSystem>();
		engine.emplaceAndRegisterSystem<Imp::RenderSystem>();
		engine.emplaceAndRegisterSystem<EditorGuiSystem>();
		engine.emplaceAndRegisterSystem<Imp::EndRenderSystem>();
		//engine.emplaceAndRegisterSystem<TestSystem>();
		//engine.emplaceAndRegisterSystem<MoreTestSystem>();
		engine.init();
		engine.run();
		//	std::cin >> str;

	//} catch (const std::exception& e) {
	//	Imp::Debug::Error("Caught exception: {}", e.what());
	//	std::cin.get();

	//}
}
