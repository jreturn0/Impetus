#pragma once
#include "BitFlags.h"
#include "Physics/PhysicsCommon.h"
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace cereal {

    //physics serialization
    template<class Archive>void serialize(Archive& archive, JPH::BodyID& v) { (archive(v.GetIndexAndSequenceNumber()), v = JPH::BodyID(v.GetIndexAndSequenceNumber())); }
    template<class Archive>void serialize(Archive& archive, imp::phys::Shape& v) { archive(v.type, v.data); }

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

    //bitflags serialization
    template<class Archive, typename T> void serialize(Archive& archive, utl::BitFlags<T>& v) {
        if constexpr (Archive::is_loading::value) {
            typename utl::BitFlags<T>::MaskType mask;
            archive(cereal::make_nvp("mask", mask));
            v = utl::BitFlags<T>(mask);
        }
        else
            archive(cereal::make_nvp("mask", v.getMask()));
    }
}
