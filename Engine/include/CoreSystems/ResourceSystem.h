#pragma once
#include "System.h"
namespace imp {


    // Unused/ not implemented 
    class ResourceSystem final : public System
    {
    public:
        void initialize(entt::registry& registry) override;        
        void update(entt::registry& registry, const float deltaTime) override;
    private:
        class ResourceManager* m_resourceManager;
    };

}