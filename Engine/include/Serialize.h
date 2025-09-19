#pragma once
#include <boost/pfr/core.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "ComponentInfoRegistry.h"
#include "Components/CameraComponent.h"
#include "Components/ModelComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/TransformComponent.h"
#include "RegisterMacro.h"
#include <cereal/types/memory.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>

#include "Components/CollisionInfoComponent.h"
#include "Components/InputStateComponent.h"
#include "Components/PhysicsBodyComponent.h"
#include "Components/Tags.h"
#include "CoreSystems/CameraSystem.h"
#include "CoreSystems/EndRenderSystem.h"
#include "CoreSystems/InputSystem.h"
#include "CoreSystems/PhysicsSystem.h"
#include "CoreSystems/PostPhysicsSystem.h"
#include "CoreSystems/PrePhysicsSystem.h"
#include "CoreSystems/RenderSystem.h"
#include "CoreSystems/ResourceSystem.h"
#include "CoreSystems/TransformSystem.h"
#include "DebugUtils/DebugGuiComponentWidgets.h"
#include "SystemRegistry.h"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "CoreSystems/RelationshipSystem.h"

namespace cereal {

}
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

#define SERIALIZE(Type) \
namespace cereal { \
    template <typename Archive> \
    void serialize(Archive& ar, Type& obj) { \
    try { \
	constexpr auto names = boost::pfr::names_as_array<Type>(); \
		boost::pfr::for_each_field(obj, [&](auto& field, size_t index) { \
            ar(cereal::make_nvp(names[index].data(), field)); \
        }); \
            } catch (const cereal::Exception& e) { \
			Debug::Error("Error serializing {}: Member {}", typeid(Type).name(), e.what()); \
              } \
    } \
}

#define REGISTER_AND_SERIALIZE(Type) \
    REGISTER_COMPONENT(Type); \
    SERIALIZE(Type)

#define REGISTER_AND_EMPTY_SERIALIZE(Type) \
    REGISTER_COMPONENT(Type); \
    EMPTY_SERIALIZE(Type)

//namespace Registration {
	// System registration
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

// Component registrationREGISTER_AND_SERIALIZE(Imp::TransformComponent);
REGISTER_AND_SERIALIZE(imp::TransformComponent);
REGISTER_AND_SERIALIZE(imp::PhysicsBodyComponent);
REGISTER_AND_SERIALIZE(imp::InputStateComponent);
REGISTER_AND_SERIALIZE(imp::CameraComponent);
REGISTER_COMPONENT_CUSTOM_WIDGET(imp::ModelComponent, ModelWidgetInfo);
SERIALIZE(imp::ModelComponent);
REGISTER_AND_SERIALIZE(imp::RelationshipComponent);
REGISTER_AND_EMPTY_SERIALIZE(imp::CollisionInfoComponent);
REGISTER_COMPONENT_CUSTOM_NAME(imp::ActiveCameraTag, "ActiveCamera")
EMPTY_SERIALIZE(imp::ActiveCameraTag);
REGISTER_COMPONENT_CUSTOM_NAME(imp::CameraLookAtTag, "CameraLookAt")
EMPTY_SERIALIZE(imp::CameraLookAtTag);

//system registration
//EMPTY_SERIALIZE(Imp::CollisionInfoComponent)
//EMPTY_SERIALIZE(Imp::ActiveCameraTag);


namespace cereal {


	//physics serialization
	template<class Archive>void serialize(Archive& archive, JPH::BodyID& v) { (archive(v.GetIndexAndSequenceNumber()), v = JPH::BodyID(v.GetIndexAndSequenceNumber())); }
	template<class Archive>void serialize(Archive& archive, imp::Phys::Shape& v) { archive(v.type, v.data); }

	//vector serialization
	template<class Archive> void serialize(Archive& archive, glm::vec2& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y)); }
	template<class Archive> void serialize(Archive& archive, glm::vec3& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z)); }
	template<class Archive> void serialize(Archive& archive, glm::vec4& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z), make_nvp("w", v.w)); }
	template<class Archive> void serialize(Archive& archive, glm::ivec2& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y)); }
	template<class Archive> void serialize(Archive& archive, glm::ivec3& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z)); }
	template<class Archive> void serialize(Archive& archive, glm::ivec4& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z), make_nvp("w", v.w)); }
	template<class Archive> void serialize(Archive& archive, glm::uvec2& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y)); }
	template<class Archive> void serialize(Archive& archive, glm::uvec3& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z)); }
	template<class Archive> void serialize(Archive& archive, glm::uvec4& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z), make_nvp("w", v.w)); }
	template<class Archive> void serialize(Archive& archive, glm::dvec2& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y)); }
	template<class Archive> void serialize(Archive& archive, glm::dvec3& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z)); }
	template<class Archive> void serialize(Archive& archive, glm::dvec4& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z), make_nvp("w", v.w)); }

	//matrix serialization
	template<class Archive> void serialize(Archive& archive, glm::mat2& m) { archive(m[0], m[1]); }
	template<class Archive> void serialize(Archive& archive, glm::dmat2& m) { archive(m[0], m[1]); }
	template<class Archive> void serialize(Archive& archive, glm::mat3& m) { archive(m[0], m[1], m[2]); }
	template<class Archive> void serialize(Archive& archive, glm::mat4& m) { archive(m[0], m[1], m[2], m[3]); }
	template<class Archive> void serialize(Archive& archive, glm::dmat4& m) { archive(m[0], m[1], m[2], m[3]); }

	//quaternion serialization
	template<class Archive> void serialize(Archive& archive, glm::quat& q) { archive(make_nvp("x", q.x), make_nvp("y", q.y), make_nvp("z", q.z), make_nvp("w", q.w)); }

}

