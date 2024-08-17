#pragma once
#include <mutex>

template<typename T>
struct CtxRef
{
private:
	//std::mutex mtx;
	T* value;
public:
	CtxRef(T* value) : value(value) {}
	CtxRef(T& value) : value(&value) {}

//	void lock() { mtx.lock(); }
	//void unlock() { mtx.unlock(); }

	T& get() { return *value; }

	T& operator*() { return *value; }
	T& operator->() { return *value; }
	
};
