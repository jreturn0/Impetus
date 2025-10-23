#pragma once
// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include "ConfigSystem.h"
#include "Debug.h"
#include "FreeList.h"
#include "FreeList.h"
#include <array>
#include <bitset>
#include <cstdarg>
#include <entt/signal/delegate.hpp>
#include <entt/signal/dispatcher.hpp>
#include <entt/signal/emitter.hpp>
#include <entt/signal/sigh.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <Jolt/Jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#include <numeric>
#include <set>
#include <string_view>
#include <string_view>
#include <thread>
#include <variant>

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS



// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

namespace imp {
    namespace phys {
        inline JPH::Vec3 ToJPH(const glm::vec3& v)
        {
            return { v.x, v.y, v.z };
        }
        inline glm::vec3 ToGLM(const JPH::Vec3& v)
        {
            return { v.GetX(), v.GetY(), v.GetZ() };
        }

        inline JPH::Vec4 ToJPH(const glm::vec4& v)
        {
            return { v.x, v.y, v.z, v.w };
        }
        inline glm::vec4 ToGLM(const JPH::Vec4& v)
        {
            return { v.GetX(), v.GetY(), v.GetZ(), v.GetW() };
        }

        inline JPH::Quat ToJPH(const glm::quat& q)
        {
            return { q.x, q.y, q.z, q.w };
        }
        inline glm::quat ToGLM(const JPH::Quat& q)
        {
            return { q.GetW(), q.GetX(), q.GetY(), q.GetZ() };
        }
        inline JPH::Mat44 ToJPH(const glm::mat4& m)
        {
            return{
                JPH::Vec4(m[0][0], m[1][0], m[2][0], m[3][0]),
                JPH::Vec4(m[0][1], m[1][1], m[2][1], m[3][1]),
                JPH::Vec4(m[0][2], m[1][2], m[2][2], m[3][2]),
                JPH::Vec4(m[0][3], m[1][3], m[2][3], m[3][3])
            };
        }
        inline glm::mat4 ToGLM(const JPH::Mat44& m)
        {
            return glm::mat4(
                m(0, 0), m(1, 0), m(2, 0), m(3, 0),
                m(0, 1), m(1, 1), m(2, 1), m(3, 1),
                m(0, 2), m(1, 2), m(2, 2), m(3, 2),
                m(0, 3), m(1, 3), m(2, 3), m(3, 3)
            );
        }
        

        // Layer that objects can be in, determines which other objects it can collide with
        // Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
        // layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
        // but only if you do collision testing).
        using ObjectLayer = JPH::ObjectLayer;
        namespace ObjectLayers {
            constexpr ObjectLayer NON_MOVING = 0;
            constexpr ObjectLayer MOVING = 1 << 0;
            constexpr ObjectLayer LAYER2 = 1 << 1;
            constexpr ObjectLayer LAYER3 = 1 << 2;
            constexpr ObjectLayer LAYER4 = 1 << 3;
            constexpr ObjectLayer LAYER5 = 1 << 4;
            constexpr ObjectLayer LAYER6 = 1 << 5;
            constexpr ObjectLayer LAYER7 = 1 << 6;
            constexpr ObjectLayer LAYER8 = 1 << 7;
            constexpr ObjectLayer LAYER9 = 1 << 8;
            constexpr ObjectLayer LAYER10 = 1 << 9;
            constexpr ObjectLayer LAYER11 = 1 << 10;
            constexpr ObjectLayer LAYER12 = 1 << 11;
            constexpr ObjectLayer LAYER13 = 1 << 12;
            constexpr ObjectLayer LAYER14 = 1 << 13;
            constexpr ObjectLayer LAYER15 = 1 << 14;
            constexpr ObjectLayer LAYER16 = 1 << 15;
            constexpr ObjectLayer DEFAULT = MOVING | LAYER2;
            constexpr size_t NUM_LAYERS = 16;
        };
        enum class MovementType : uint8_t
        {
            Static = JPH::EMotionType::Static,
            Kinematic = JPH::EMotionType::Kinematic,
            Dynamic = JPH::EMotionType::Dynamic,
            Count = 3
        };
        enum class ShapeType : uint8_t
        {
            None = 0,
            Box = 1 << 0,
            Sphere = 1 << 1,
            Capsule = 1 << 2,
            Cylinder = 1 << 3,
            Count = 4
        };
        struct Shape
        {
            ShapeType type{ ShapeType::Box };
            glm::vec3 data{ 1.f };


            bool operator==(const Shape& other) const
            {
                return type == other.type && data == other.data;
            }
        };
        namespace BroadPhaseLayers {
            static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
            static constexpr JPH::BroadPhaseLayer MOVING(1);
            static constexpr JPH::uint NUM_LAYERS(2);
        };

        constexpr JPH::uint64 ShapeTypeMask = 0b1111;

        constexpr std::string_view ToString(ObjectLayer inLayer, bool name = true)
        {
            switch (inLayer) {
            case ObjectLayers::NON_MOVING:	return name ? "NonMoving" : "0";
            case ObjectLayers::MOVING:		return name ? "Moving" : "1";
            case ObjectLayers::LAYER2:		return name ? "Layer2" : "2";
            case ObjectLayers::LAYER3:		return name ? "Layer3" : "3";
            case ObjectLayers::LAYER4:		return name ? "Layer4" : "4";
            case ObjectLayers::LAYER5:		return name ? "Layer5" : "5";
            case ObjectLayers::LAYER6:		return name ? "Layer6" : "6";
            case ObjectLayers::LAYER7:		return name ? "Layer7" : "7";
            case ObjectLayers::LAYER8:		return name ? "Layer8" : "8";
            case ObjectLayers::LAYER9:		return name ? "Layer9" : "9";
            case ObjectLayers::LAYER10:		return name ? "Layer10" : "10";
            case ObjectLayers::LAYER11:		return name ? "Layer11" : "11";
            case ObjectLayers::LAYER12:		return name ? "Layer12" : "12";
            case ObjectLayers::LAYER13:		return name ? "Layer13" : "13";
            case ObjectLayers::LAYER14:		return name ? "Layer14" : "14";
            case ObjectLayers::LAYER15:		return name ? "Layer15" : "15";
            case ObjectLayers::LAYER16:		return name ? "Layer16" : "16";
            default:						return "Invalid";
            }
        }
        constexpr std::string_view ToString(MovementType inType)
        {
            switch (inType) {
            case MovementType::Static:	return "Static";
            case MovementType::Dynamic:	return "Dynamic";
            case MovementType::Kinematic:	return "Kinematic";
            default: return "Invalid";
            }
        }
        constexpr std::string_view ToString(ShapeType inType)
        {
            switch (inType) {
            case ShapeType::None:		return "None";
            case ShapeType::Box:		return "Box";
            case ShapeType::Sphere:		return "Sphere";
            case ShapeType::Capsule:	return "Capsule";
            case ShapeType::Cylinder:	return "Cylinder";
            default:					return "Invalid";
            }
        }
        constexpr JPH::BroadPhaseLayer CastBroadPhaseLayer(bool inLayer) { return static_cast<JPH::BroadPhaseLayer>(inLayer); }
    }
}


