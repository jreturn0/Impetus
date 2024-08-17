# Impetus
Engine/Framework made with C++/Vulkan
vcpkg and the vulkan sdk are needed to run/build the project
# Tech
- ## C++ / Vulkan
	- ### Libraries
		- #### [EnTT](https://github.com/skypjack/entt)
			- Primary base of the engine's architecture. Used mostly for it's ecs(entity component system), containers, and events/signals/delegates.   
		- #### [Boost.PFR](https://github.com/boostorg/pfr)
			- Used to mostly to do basic automation of a lot of reflection and serialization
		- #### [Cereal](https://github.com/USCiLab/cereal)
			- Used for serialization
		- #### [Jolt Physics](https://github.com/jrouwe/JoltPhysics)
			- Physics and Collision. Kept a little bit more in line with my paradigm than others  			
		- #### [GLM](https://github.com/g-truc/glm)
			- Primary math library. Used for familiarity and compatibility with glsl  
		- #### [VulkanHPP](https://github.com/KhronosGroup/Vulkan-Hpp)
			- C++ wrapper around Vulkan. Used mostly for convenience and clarity for myself
		- #### [Dear ImGui](https://github.com/ocornut/imgui)
			- GUI. Used for debugging purposes and the editor 
		- #### [Shaderc](https://github.com/google/shaderc)
			- Used for compiling shaders. Specifically here glsl to spir-v 
		- #### [SPIRV-Reflect](https://github.com/KhronosGroup/SPIRV-Reflect)
			- Currently unused aside from printing while compiling
- ## Software
    - ### Visual Studio
        - #### Programming IDE
    - ### Blender
        - #### 3D Modelling. Mostly used to build the glb(gltf) files
    
