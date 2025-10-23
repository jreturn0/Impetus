#pragma once
#include "ComponentInfoRegistry.h"
#include "Components/CameraComponent.h"
#include "Components/CameraLookAtComponent.h"
#include "Components/CollisionInfoComponent.h"
#include "Components/LightComponent.h"
#include "Components/ModelComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/Tags.h"
#include "Components/TransformComponent.h"
#include "CoreSerialize.h"
#include "CoreSystems/CameraSystem.h"
#include "CoreSystems/EndRenderSystem.h"
#include "CoreSystems/InputSystem.h"
#include "CoreSystems/PhysicsSystem.h"
#include "CoreSystems/PostPhysicsSystem.h"
#include "CoreSystems/PrePhysicsSystem.h"
#include "CoreSystems/RelationshipSystem.h"
#include "CoreSystems/RenderSystem.h"
#include "CoreSystems/ResourceSystem.h"
#include "CoreSystems/TransformSystem.h"
#include "Physics/PhysicsCommon.h"
#include "RegisterMacro.h"
#include "SerializeTypes.h"
#include "SystemManager.h"
#include "SystemRegistry.h"
#include <boost/pfr.hpp>









// Empty serialize function
template <typename Type>
void EmptySerialize(cereal::JSONOutputArchive& ar, Type& obj)
{
    // Do nothing
}

#define EMPTY_SERIALIZE(Type) \
namespace cereal { \
    template <typename Archive> \
    void serialize(Archive& ar, Type& obj) { \
    } \
}

namespace imp {
    template <typename T, typename Archive>
    void reflectSerialize(Archive& ar, T& obj) {
        constexpr auto names = boost::pfr::names_as_array<T>();
        std::size_t index = 0;
        boost::pfr::for_each_field(obj, [&](auto& field) {
            const std::string name{ names[index] };
            if constexpr (Archive::is_loading::value) {
                try {
                    ar(cereal::make_nvp(name.c_str(), field));
                }
                catch (const cereal::Exception&) {
                    Debug::Info("Component deserialization: Field '{}' not found, using default value.", name);
                }
            }
            else {
                ar(cereal::make_nvp(name.c_str(), field));
            }

            ++index;
            });
    }
}

#define SERIALIZE(Type) \
namespace cereal {  \
    template <typename Archive > \
    void serialize(Archive& ar, Type& obj) { \
        imp::reflectSerialize(ar, obj); \
    } \
} \


#define REGISTER_AND_SERIALIZE(Type) \
    REGISTER_COMPONENT(Type); \
    SERIALIZE(Type)

#define REGISTER_AND_EMPTY_SERIALIZE(Type) \
    REGISTER_COMPONENT(Type); \
    EMPTY_SERIALIZE(Type)

// Register core systems
REGISTER_SYSTEM(imp::RenderSystem)
REGISTER_SYSTEM(imp::ResourceSystem)
REGISTER_SYSTEM(imp::InputSystem)
REGISTER_SYSTEM(imp::TransformSystem)
REGISTER_SYSTEM(imp::PrePhysicsSystem)
REGISTER_SYSTEM(imp::PhysicsSystem)
REGISTER_SYSTEM(imp::PostPhysicsSystem)
REGISTER_SYSTEM(imp::EndRenderSystem)
REGISTER_SYSTEM(imp::CameraSystem)
REGISTER_SYSTEM(imp::RelationshipSystem)

// Register core components
REGISTER_AND_SERIALIZE(imp::TransformComponent);
REGISTER_AND_SERIALIZE(imp::PhysicsBodyComponent);
REGISTER_AND_SERIALIZE(imp::CameraComponent);
REGISTER_AND_SERIALIZE(imp::ModelComponent);
REGISTER_AND_SERIALIZE(imp::RelationshipComponent);
REGISTER_AND_EMPTY_SERIALIZE(imp::CollisionInfoComponent);
REGISTER_COMPONENT_CUSTOM_NAME(imp::ActiveCameraTag, "ActiveCamera")
EMPTY_SERIALIZE(imp::ActiveCameraTag);
REGISTER_AND_SERIALIZE(imp::CameraLookAtComponent)
REGISTER_AND_SERIALIZE(imp::LightComponent)




