#pragma once
#include <unordered_map>
#include <functional>
#include <memory>

#include "Debug.h"
#include "System.h"
#include "RegisterMacro.h"

namespace imp {

	class SystemRegistry
	{
	private:
		inline static std::unordered_map<std::string, std::function<std::unique_ptr<System>()>> systemFactory{};

	public:
		inline static std::unordered_map<std::string, std::function<std::unique_ptr<System>()>>& GetSystemFactory() { return systemFactory; };
		template<typename T>
		inline static void RegisterSystem()
		{
			std::string name = typeid(T).name();
			name = name.substr(6);
			systemFactory.insert_or_assign(name, []() {return std::make_unique<T>(); });
		}
	};
	// Register system function
	namespace Register {
		template <typename Type>
		struct SystemRegistrar
		{
			SystemRegistrar()
			{
				try {
					imp::SystemRegistry::RegisterSystem<Type>();
				}
				catch (const std::exception& e) {
					Debug::Exception("Error registering System: {}: {}", typeid(Type).name(), e.what());
				
				}
			}
		};
	}

#define REGISTER_SYSTEM(Type) \
	inline static imp::Register::SystemRegistrar<Type> UNIQUE_NAME(autoRegisterInstance_){};
}





