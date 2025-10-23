#pragma once
#include <entt/resource/resource.hpp>
#include <string>
enum ResourceType
{
	Texture,
	Mesh,
	Shader,
	Audio,
	Scene,
	Test
}; 



template<typename T>
struct ResourceMember
{
	std::string path;
	ResourceType type;
	bool loaded = false;
	entt::resource<T> handle;
};