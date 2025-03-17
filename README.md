# Impetus
Engine/Framework made with C++/Vulkan

#### Description/History
Made originally as a capstone project. Currently rewriting much of the architecture 

###### Note
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
		- #### [STB](https://github.com/nothings/stb)
			- Image loading
		- #### [FastGltf](https://github.com/spnda/fastgltf)
		  	- For storing and loading meshes/textures from primary data type .glb(gltf)
- ## Software
	- ### Visual Studio
   		- Programming IDE
	- ### Blender
   		- 3D Modelling. Mostly used to build the glb(gltf) files
- ## Sources
	- ### [FreePBR](https://freepbr.com/)
   		- where I got some basic textures
	- ### [imgui_entt_entity_editor](https://github.com/Green-Sky/imgui_entt_entity_editor)
	  	- Basis of a lot of the editor through concept. Not directly used but got a lot of core concepts
   	- ### [Vulkan Tutorial](https://vulkan-tutorial.com/)
   	  	- Learning Vulkan and more related concepts. Used to help abstract core Vulkan concepts and items
   	- ### [vkguide](https://vkguide.dev/)
   	  	- Same as above. Mostly used as basis for the renderer's architecture
  
