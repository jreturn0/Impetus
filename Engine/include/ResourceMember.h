#pragma once
#include <string>
#include <entt/resource/resource.hpp>
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