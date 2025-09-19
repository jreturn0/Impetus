#pragma once
#include <execution>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

namespace imp
{
	template<typename T, typename F>
	void ForEachParallel(T& contiguousContainer, F&& function)
	{
		std::for_each(std::execution::par_unseq, contiguousContainer.begin(), contiguousContainer.end(), function);
	}
}

class System
{
public:
	virtual ~System() = default;

	virtual void initialize(entt::registry& registry) {};
	virtual void update(entt::registry& registry, const float deltaTime) = 0;
	virtual void cleanup(entt::registry& registry) {};


};