#pragma once
#include <mutex>

// A simple wrapper to store references in the entt system locator 
template<typename T>
struct CtxRef
{
private:
	T* value;
public:
	CtxRef(T* value) : value(value) {}
	CtxRef(T& value) : value(&value) {}

	T& get() { return *value; }

	T& operator*() { return *value; }
	T& operator->() { return *value; }
	
};
