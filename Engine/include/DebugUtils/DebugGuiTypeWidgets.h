
#pragma once
#ifdef _DEBUG
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <array>
#include <map>
#include <string>
#include <cstring>
#include "Physics/PhysicsCommon.h"
#include "CameraType.h"
//#include "../Resources/ResourceMetadata.h"

namespace Imp {
	template<typename T>
	inline void TypeWidget(T& t, const std::string& name);


	template<typename T>
	inline void TypeWidget(T& t, const std::string& name)
	{
		//if constexpr (std::is_class_v<T>) {
		//    if (ImGui::TreeNode(name.c_str())) {
		//        boost::pfr::for_each_field(t, [&](auto& field, auto idx) {
		//            constexpr auto field_names = boost::pfr::names_as_array<T>();
		//            TypeWidget(field, field_names[idx]);
		//                                   });
		//        ImGui::TreePop();
		//    }
		//} else {
		ImGui::Text("%s: %s", name.c_str(), typeid(T).name()); // Default handler
		//        }
	}
	template<>
	inline void TypeWidget(entt::id_type& t, const std::string& name)
	{
		
		ImGui::Text("%s: %i", name.c_str(), entt::to_integral(t));
	}
	template<>
	inline void TypeWidget(entt::entity& t, const std::string& name)
	{
		int id = entt::to_integral(t);
		ImGui::InputInt(name.c_str(), &id);
		t= static_cast<entt::entity>(id);
	}

	template<>
	inline void TypeWidget<Phys::MovementType>(Phys::MovementType& t, const std::string& name)
	{
		if (ImGui::Button(Phys::ToString(t).data())) {
			ImGui::OpenPopup("MovementType");
		}

		if (ImGui::BeginPopup("MovementType")) {
			for (int i = 0; i < static_cast<int>(Phys::MovementType::Count); ++i) {
				if (const auto type = static_cast<Phys::MovementType>(i); ImGui::Selectable(Phys::ToString(type).data())) {
					t = type;
				}
			}
			ImGui::EndPopup();
		}
	}
	template<>
	inline void TypeWidget<Imp::CameraType>(Imp::CameraType& t, const std::string& name)
	{
		
		if (ImGui::Button(Imp::ToString(t).data())) {
			ImGui::OpenPopup("CameraType");
		}

		if (ImGui::BeginPopup("CameraType")) {
			for (int i = 0; i < (2); ++i) {
				if (const auto type = static_cast<CameraType>(i); ImGui::Selectable(Imp::ToString(type).data())) {
					t = type;
				}
			}
			ImGui::EndPopup();
		}
	}

	template<>
	inline void TypeWidget<JPH::BodyID>(JPH::BodyID& t, const std::string& name)
	{
		ImGui::Text("BodyID: %ui(Index:%ui)(Sequence:%ui)", t.GetIndexAndSequenceNumber(), t.GetIndex(), t.GetSequenceNumber());
	}

	template<>
	inline void TypeWidget<Phys::ObjectLayer>(Phys::ObjectLayer& t, const std::string& name)
	{

		std::bitset<16> bits(t);
		constexpr int rowTotal = 2;
		constexpr int colTotal = 8;
		constexpr int colSpace = 4;

		int currentRow = 0;
		int currentCol = 0;

		for (unsigned i = 0; i < Phys::ObjectLayers::NUM_LAYERS; i++) {
			bool bit = bits[i];
			ImGui::PushID(i);
			if (ImGui::Checkbox("", &bit))
			{
				bits[i] = bit; 
			}
			ImGui::PopID();
			ImGui::SetItemTooltip("%s", Phys::ToString(static_cast<Phys::ObjectLayer>(1 << i)).data());


			if ((currentCol + 1) % colSpace == 0 && currentCol != colTotal - 1) {
				ImGui::SameLine(0, 20.0f); // Adjust the second parameter to control the space between groups
			} else if (currentCol < colTotal - 1) {
				ImGui::SameLine();
			}

			currentCol++;
			if (currentCol >= colTotal) {
				currentCol = 0;
				currentRow++;
			}

		}

		t = static_cast<Phys::ObjectLayer>(bits.to_ulong());

	}
	template<>
	inline void TypeWidget(Phys::Shape& t, const std::string& name)
	{
		if (ImGui::Button(Phys::ToString(t.type).data())) {
			ImGui::OpenPopup("ShapeType");
		}

		if (ImGui::BeginPopup("ShapeType")) {
			for (int i = 0; i < static_cast<int>(Phys::ShapeType::Count); ++i) {
				if (const auto type = static_cast<Phys::ShapeType>(i); ImGui::Selectable(Phys::ToString(type).data())) {
					t.type = type;
				}
			}
			ImGui::EndPopup();
		}
		switch (t.type) {
		case Phys::ShapeType::None:
			break;
		case Phys::ShapeType::Box:
			{
				ImGui::DragFloat3("Half Extents", glm::value_ptr(t.data), 0.01f,0.1f);
			}
			break;
		case Phys::ShapeType::Sphere:
			{
				ImGui::DragFloat("Radius", &t.data.x, 0.01f,0.1f);
			}
			break;
		case Phys::ShapeType::Capsule:
		case Phys::ShapeType::Cylinder:
			{
				ImGui::DragFloat("Radius", &t.data.x, 0.01f,0.01f);
				ImGui::DragFloat("Half Height", &t.data.y, 0.01f,0.1f);
			}
			break;
		}

	}

	template<>
	inline void TypeWidget(int& t, const std::string& name)
	{
		ImGui::DragInt(name.c_str(), &t, 0.5f);
	}

	template<>
	inline void TypeWidget(float& t, const std::string& name)
	{
		ImGui::DragFloat(name.c_str(), &t, 0.01f);
	}

	template<>
	inline void TypeWidget(double& t, const std::string& name)
	{
		ImGui::DragFloat(name.c_str(), reinterpret_cast<float*>(&t), 0.01f);
	}

	template<>
	inline void TypeWidget(bool& t, const std::string& name)
	{
		ImGui::Checkbox(name.c_str(), &t);
	}

	template<>
	inline void TypeWidget(char& t, const std::string& name)
	{
		char buffer[2] = { t, '\0' };
		if (ImGui::InputText(name.c_str(), buffer, 2)) {
			t = buffer[0];
		}
	}

	template<>
	inline void TypeWidget(std::string& t, const std::string& name)
	{
		char buffer[256];
		strncpy_s(buffer, t.c_str(), sizeof(buffer));
		buffer[sizeof(buffer) - 1] = '\0';
		if (ImGui::InputText(name.c_str(), buffer, sizeof(buffer))) {
			t = buffer;
		}
	}

	template<typename T>
	inline void TypeWidget(std::vector<T>& vec, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			for (size_t i = 0; i < vec.size(); ++i) {
				TypeWidget(vec[i], name + "[" + std::to_string(i) + "]");
			}
			ImGui::TreePop();
		}
	}

	template<typename T, size_t N>
	inline void TypeWidget(std::array<T, N>& arr, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			for (size_t i = 0; i < N; ++i) {
				TypeWidget(arr[i], name + "[" + std::to_string(i) + "]");
			}
			ImGui::TreePop();
		}
	}

	template<typename Key, typename Value>
	inline void TypeWidget(std::map<Key, Value>& map, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			for (auto& [key, value] : map) {
				std::string keyName = name + "[Key]";
				TypeWidget(key, keyName);
				TypeWidget(value, name + "[Value]");
			}
			ImGui::TreePop();
		}
	}


	// Implementation for std::unordered_set
	template<typename T>
	inline void TypeWidget(std::unordered_set<T>& uset, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			size_t i = 0;
			for (auto& elem : uset) {
				TypeWidget<T>(elem, name + "[" + std::to_string(i) + "]");
				++i;
			}
			ImGui::TreePop();
		}
	}	template<>
	inline void TypeWidget(std::unordered_set<entt::entity>& uset, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			size_t i = 0;
			for (auto& elem : uset) {
				ImGui::Text("Entity: %i", entt::to_integral(elem));
				++i;
			}
			ImGui::TreePop();
		}
	}

	// Implementation for std::unordered_map
	template<typename Key, typename Value>
	inline void TypeWidget(std::unordered_map<Key, Value>& umap, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			for (auto& [key, value] : umap) {
				TypeWidget(key, name + "[Key]");
				TypeWidget(value, name + "[Value]");
			}
			ImGui::TreePop();
		}
	}

	// Implementation for std::list
	template<typename T>
	inline void TypeWidget(std::list<T>& lst, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			size_t i = 0;
			for (auto& elem : lst) {
				TypeWidget<entt::entity>(elem, name + "[" + std::to_string(i) + "]");
				++i;
			}
			ImGui::TreePop();
		}
	}


	// glm types
	template<>
	inline void TypeWidget(glm::vec2& v, const std::string& name)
	{
		ImGui::DragFloat2(name.c_str(), glm::value_ptr(v), 0.01f);
	}

	template<>
	inline void TypeWidget(glm::vec3& v, const std::string& name)
	{
		ImGui::DragFloat3(name.c_str(), glm::value_ptr(v), 0.01f);
	}

	template<>
	inline void TypeWidget(glm::vec4& v, const std::string& name)
	{
		ImGui::DragFloat4(name.c_str(), glm::value_ptr(v), 0.01f);
	}

	template<>
	inline void TypeWidget(glm::ivec2& v, const std::string& name)
	{
		ImGui::DragInt2(name.c_str(), glm::value_ptr(v), 0.01f);
	}

	template<>
	inline void TypeWidget(glm::ivec3& v, const std::string& name)
	{
		ImGui::DragInt3(name.c_str(), glm::value_ptr(v), 0.01f);
	}

	template<>
	inline void TypeWidget(glm::ivec4& v, const std::string& name)
	{
		ImGui::DragInt4(name.c_str(), glm::value_ptr(v), 0.01f);
	}

	template<>
	inline void TypeWidget(glm::dvec2& v, const std::string& name)
	{
		ImGui::DragFloat2(name.c_str(), reinterpret_cast<float*>(glm::value_ptr(v)), 0.01f);
	}

	template<>
	inline void TypeWidget(glm::dvec3& v, const std::string& name)
	{
		ImGui::DragFloat3(name.c_str(), reinterpret_cast<float*>(glm::value_ptr(v)), 0.01f);
	}

	template<>
	inline void TypeWidget(glm::dvec4& v, const std::string& name)
	{
		ImGui::DragFloat4(name.c_str(), reinterpret_cast<float*>(glm::value_ptr(v)), 0.01f);
	}

	template<>
	inline void TypeWidget(glm::mat2& m, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			for (int i = 0; i < 2; ++i) {
				TypeWidget(m[i], name + "[" + std::to_string(i) + "]");
			}
			ImGui::TreePop();
		}
	}

	template<>
	inline void TypeWidget(glm::mat3& m, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			for (int i = 0; i < 3; ++i) {
				TypeWidget(m[i], name + "[" + std::to_string(i) + "]");
			}
			ImGui::TreePop();
		}
	}

	template<>
	inline void TypeWidget(glm::mat4& m, const std::string& name)
	{
		if (ImGui::TreeNode(name.c_str())) {
			for (int i = 0; i < 4; ++i) {
				TypeWidget(m[i], name + "[" + std::to_string(i) + "]");
			}
			ImGui::TreePop();
		}
	}

	template<>
	inline void TypeWidget(glm::quat& q, const std::string& name)
	{
		glm::vec3 euler = glm::eulerAngles(q);
		euler = glm::degrees(euler); 

		if (ImGui::DragFloat3((name + " (Pitch, Yaw, Roll)").c_str(), glm::value_ptr(euler), 0.01f, -360.0f, 360.0f)) {
			euler = glm::radians(euler); 
			q = glm::quat(euler); 
		}
	}

	template<>
	inline void TypeWidget(entt::dense_set<entt::entity>& q, const std::string& name)
	{
		for (auto& elem : q) {
			ImGui::Text("Entity: %i", entt::to_integral(elem));
		}


	}

	template<>
	inline void TypeWidget(glm::dquat& q, const std::string& name)
	{
		glm::dvec3 euler = glm::eulerAngles(q);
		euler = glm::degrees(euler); // Convert to degrees for more intuitive editing

		// ImGui::DragFloat3 works with float, so we need to cast to float*
		if (ImGui::DragFloat3((name + " (Pitch, Yaw, Roll)").c_str(), reinterpret_cast<float*>(glm::value_ptr(euler)), 0.01f, -360.0f, 360.0f)) {
			euler = glm::radians(euler); // Convert back to radians
			q = glm::dquat(euler); // Recreate quaternion from Euler angles
		}
	}
	//template<typename T>
	//inline void TypeWidget(typename ResourceMetadata<T>::Type& type, const std::string& name)
	//{
	//    const char* typeNames[] = { "TEXTURE", "MESH", "SHADER", "AUDIO", "SCENE" };
	//    int typeIndex = static_cast<int>(type);
	//    if (ImGui::Combo(name.c_str(), &typeIndex, typeNames, IM_ARRAYSIZE(typeNames))) {
	//        type = static_cast<typename ResourceMetadata<T>::Type>(typeIndex);
	//    }
	//}

	//template<typename T>
	//inline void TypeWidget(ResourceMetadata<T>& metadata, const std::string& name)
	//{
	//    if (ImGui::TreeNode(name.c_str())) {
	//        TypeWidget(metadata.path, "Path");
	//        TypeWidget(metadata.type, "Type");
	//        // Skip `loaded` and `handle`
	//        ImGui::TreePop();
	//    }
	//}
}
#endif