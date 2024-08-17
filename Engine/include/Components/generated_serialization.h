//#pragma once
//#include <cereal/archives/json.hpp>
//#include <cereal/types/vector.hpp>
//#include <cereal/types/string.hpp>
//#include "../Serialize.h"
//
//#include "CameraComponent.h"
//#include "CollisionInfoComponent.h"
//#include "InputStateComponent.h"
//#include "ModelComponent.h"
//#include "PhysicsBodyComponent.h"
//#include "RelationshipComponent.h"
//#include "TransformComponent.h"
//
//namespace cereal {
//template<typename Archive>
//inline void serialize(Archive& archive, Imp::CameraComponent& component) {
//    archive(cereal::make_nvp("fov", component.fov));
//    archive(cereal::make_nvp("nearPlane", component.nearPlane));
//    archive(cereal::make_nvp("farPlane", component.farPlane));
//    archive(cereal::make_nvp("pitch", component.pitch));
//    archive(cereal::make_nvp("yaw", component.yaw));
//}
//}
//
//namespace cereal {
//template<typename Archive>
//inline void serialize(Archive& archive, Imp::CollisionInfoComponent& component) {
//    archive(cereal::make_nvp("collidingEntities", component.collidingEntities));
//}
//}
//
//namespace cereal {
//template<typename Archive>
//inline void serialize(Archive& archive, Imp::InputStateComponent& component) {
//    archive(cereal::make_nvp("keyStates", component.keyStates));
//    archive(cereal::make_nvp("mouseButtonStates", component.mouseButtonStates));
//    archive(cereal::make_nvp("mouseX", component.mouseX));
//    archive(cereal::make_nvp("mouseY", component.mouseY));
//    archive(cereal::make_nvp("lastMouseX", component.lastMouseX));
//    archive(cereal::make_nvp("lastMouseY", component.lastMouseY));
//    archive(cereal::make_nvp("scrollX", component.scrollX));
//    archive(cereal::make_nvp("scrollY", component.scrollY));
//    archive(cereal::make_nvp("cursorHidden", component.cursorHidden));
//}
//}
//
//namespace cereal {
//template<typename Archive>
//inline void serialize(Archive& archive, Imp::ModelComponent& component) {
//    archive(cereal::make_nvp("gltf", component.gltf));
//    archive(cereal::make_nvp("name", component.name));
//    archive(cereal::make_nvp("localTransform", component.localTransform));
//    archive(cereal::make_nvp("materialOverrideName", component.materialOverrideName));
//    archive(cereal::make_nvp("visible", component.visible));
//}
//}
//
//namespace cereal {
//template<typename Archive>
//inline void serialize(Archive& archive, Imp::PhysicsBodyComponent& component) {
//    archive(cereal::make_nvp("movementType", component.movementType));
//    archive(cereal::make_nvp("layer", component.layer));
//    archive(cereal::make_nvp("shape", component.shape));
//    archive(cereal::make_nvp("linearVelocity", component.linearVelocity));
//    archive(cereal::make_nvp("angularVelocity", component.angularVelocity));
//    archive(cereal::make_nvp("friction", component.friction));
//    archive(cereal::make_nvp("restitution", component.restitution));
//    archive(cereal::make_nvp("id", component.id));
//    archive(cereal::make_nvp("forceUpdateShape", component.forceUpdateShape));
//}
//}
//
//namespace cereal {
//template<typename Archive>
//inline void serialize(Archive& archive, Imp::RelationshipComponent& component) {
//    archive(cereal::make_nvp("parent", component.parent));
//    archive(cereal::make_nvp("children", component.children));
//}
//}
//
//namespace cereal {
//template<typename Archive>
//inline void serialize(Archive& archive, Imp::TransformComponent& component) {
//    archive(cereal::make_nvp("position", component.position));
//    archive(cereal::make_nvp("rotation", component.rotation));
//    archive(cereal::make_nvp("scale", component.scale));
//    archive(cereal::make_nvp("dirty", component.dirty));
//}
//}
//
