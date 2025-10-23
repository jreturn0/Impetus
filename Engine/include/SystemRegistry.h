#pragma once
#include "Debug.h"
#include "RegisterMacro.h"
#include "System.h"
#include "TypeName.h"
#include <functional>
#include <memory>
#include <unordered_map>

namespace imp {

    class SystemRegistry
    {
        using SystemFactoryMap = std::unordered_map<utl::StringHash, std::function<std::unique_ptr<System>()>,utl::StringHashOp,std::equal_to<>>;
    public:
        inline static SystemFactoryMap& GetSystemFactory() { return m_systemFactory; };
        template<typename T>
        inline static std::string_view RegisterSystem()
        {
            constexpr std::string_view name = utl::TypeName<T>::name;
            
            if (m_systemFactory.insert_or_assign(name, []() {return std::make_unique<T>(); }).second) {
                Debug::Info("Registered System: {}", name);
            }
            return name;
        }
    private:
        inline static SystemFactoryMap m_systemFactory{};

    };
    namespace Register {
    // Register system function
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





