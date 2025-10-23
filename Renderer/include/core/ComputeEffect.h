#pragma once
#include "utils/QuickMacros.h"
#include <glm/vec4.hpp>
#include <string>

namespace imp {
    // Base class for compute effects
    class ComputeEffect
    {
    public:
        struct PushConstants
        {
            glm::vec4 data1 = { 1.f,0.f,0.f,1.f };
            glm::vec4 data2 = { 0.f,1.f,0.f,1.f };
            glm::vec4 data3 = { 0.f,0.f,1.f,1.f };
            glm::vec4 data4 = { 0.f,0.f,0.f,1.f };
            glm::vec4 & operator[](size_t index) noexcept
            {
                switch (index)
                {
                case 0: return data1;
                case 1: return data2;
                case 2: return data3;
                case 3: return data4;
                default: return data1;
                }
            }
            const glm::vec4& operator[](size_t index) const noexcept
            {
                switch (index)
                {
                case 0: return data1;
                case 1: return data2;
                case 2: return data3;
                case 3: return data4;
                default: return data1;
                }
            }
        };

        ComputeEffect(std::string_view name) : pushes(), name(name) {}
        ComputeEffect() = default;
        ~ComputeEffect() = default;
        ComputeEffect(const ComputeEffect&) = delete; 
        ComputeEffect& operator=(const ComputeEffect&) = delete; 
        ComputeEffect(ComputeEffect&&) = delete; 
        ComputeEffect& operator=(ComputeEffect&&) = delete;

        // Getters and Setters

        inline PushConstants& getPushConstants() noexcept { return pushes; }
        inline const PushConstants& getPushConstants() const noexcept { return pushes; }
        inline void setPushConstants(const PushConstants& data) noexcept { pushes = data; }
        inline void setPushConstants(const glm::vec4& data1, const glm::vec4& data2, const glm::vec4& data3, const glm::vec4& data4) noexcept { pushes = { data1, data2, data3, data4 }; }
        inline std::string_view getName() const noexcept  { return name; }

    protected:
        PushConstants pushes{};
        std::string name{};
    };

}