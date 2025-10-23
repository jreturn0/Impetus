#define GLM_ENABLE_EXPERIMENTAL
#pragma comment(lib, "opengl32.lib")
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "RNG.h"
#include "TimerStats.h"
#include <chrono>
#include <entt/entt.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/unordered_map.hpp>
#include <functional>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <iostream>

struct TransformComponent {
    glm::vec3 position{ 0.f,0.f,0.f };
    glm::quat rotation{ 1.f, 0.f,0.f,0.f };
    glm::vec3 scale{ 1.f,1.f,1.f };
};

struct ModelComponent {
    std::string modelPath;
    glm::mat4 modelMatrix{ 1.f };
};

class EditReg {
public:
    template<typename T>
    static void Inject() {
        std::cout << "Injected: " << entt::type_id<T>().name() << std::endl;
    }
};


// Customization point namespace for registration hooks
namespace compreg_ext {
    template<typename T>
    struct hook_tag {};
    // Note: no declarations here; CompReg does not know anything about EditReg.
}

struct ComponentInfo {
    std::string name;
    std::function<void(entt::snapshot&, cereal::JSONOutputArchive&)> serialize;
    std::function<void(entt::snapshot_loader&, cereal::JSONInputArchive&)> deserialize;
};

struct CompReg {
    inline static std::unordered_map<entt::id_type, size_t> runtimeInfo{};

    template<typename T>
    static void Register() {
        const entt::id_type id = entt::type_id<T>().hash();
        runtimeInfo[id] = sizeof(T);

        using compreg_ext::hook_tag;

        // If an editor-side hook is provided, ADL will find and call it.
        if constexpr (requires { on_component_registered(hook_tag<T>{}); }) {
            on_component_registered(hook_tag<T>{});
        }
    }
};

#define REGISTER_COMPONENT(Type) \
    inline static struct { \
        decltype(auto) dummy = (CompReg::Register<Type>(), 0); \
    } UNIQUE_NAME(autoRegisterInstance_);

// ----------------------
// Editor-side opt-in:
// Provide the hook in compreg_ext namespace so ADL finds it.
// This can live in the editor module/translation unit.
// ----------------------
namespace compreg_ext {
    template<typename T>
    void on_component_registered(hook_tag<T>) {
        EditReg::Inject<T>();
    }
}

int main() {
    // Demo: register a couple of components
    CompReg::Register<TransformComponent>();
    CompReg::Register<ModelComponent>();
    return 0;
}