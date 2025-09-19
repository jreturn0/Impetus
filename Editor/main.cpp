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
    auto engine = imp::Impetus("Impetus", 60);
    //engine.registerResourceAndLoader<MyResource, MyResourceLoader>();
    engine.emplaceAndRegisterSystem<imp::InputSystem>();
    engine.emplaceAndRegisterSystem<imp::ResourceSystem>();
    engine.emplaceAndRegisterSystem<imp::TransformSystem>();
    engine.emplaceAndRegisterSystem<imp::PrePhysicsSystem>(imp::UpdateType::PreFixed);
    engine.emplaceAndRegisterSystem<imp::PhysicsSystem>(imp::UpdateType::Fixed);
    engine.emplaceAndRegisterSystem<imp::PostPhysicsSystem>(imp::UpdateType::PostFixed);
    engine.emplaceAndRegisterSystem<CameraControllerSystem>();
    engine.emplaceAndRegisterSystem<imp::CameraSystem>();
    engine.emplaceAndRegisterSystem<imp::RenderSystem>();
    engine.emplaceAndRegisterSystem<EditorGuiSystem>();
    engine.emplaceAndRegisterSystem<imp::EndRenderSystem>();
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
