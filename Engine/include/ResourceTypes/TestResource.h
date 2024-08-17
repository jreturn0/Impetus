#pragma once
#include <memory>
#include "ResourceRegistry.h"
#include <Debug.h>


struct TestResource
{
	int* data;
	size_t size;

	~TestResource() { Debug::Info("Deleting data({}) of size {}", *data, size); delete data;  }
};


struct TestResourceLoader final
{
	using result_type = std::shared_ptr<TestResource>;

	result_type operator()(const char* value) const
	{
		return std::make_shared<TestResource>(new int(42 ), 12345678);
	}
};


REGISTER_RESOURCE(TestResource, TestResourceLoader)