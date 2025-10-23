# Impetus
Engine/Framework made with C++/Vulkan

#### Description/History
Made originally as a capstone project in 2024. Picked back up in 2025 to clean up it up what already exists.


#### Recent Major changes
Removed a lot of redundant code and tried my best to optimize things. 
- The save/load "scene" format was changed to be more stable. Adding and removing components from being serialized should no longer break loads with different sets of serialized components. Default values are used on fields where they do not match or exist in the original serialized data
- Moved to VulkanSDK 1.4.313.2
- Removed and congregated a lot of minor wrappers in the renderer, adding a single "context" object in their place or using the raw types
- Added point and directional lights to the renderer based off of [LearnOpenGL's Lighting section](https://learnopengl.com/Lighting/Multiple-lights)
- Improved the mesh_material shading based off of [LearnOpenGL's PBR lighting section](https://learnopengl.com/PBR/Lighting)
- Option to hot recompile shaders, pausing on errors
- Multithreaded immediate/single-submit commands for faster glb/image load times
- Image format choosing for swap chain, draw, and present images was made consistent. Choosing preferred formats if available
- Added a .ini based config system. largely inspired by [vkguide's "CVAR System"](https://vkguide.dev/docs/extra-chapter/cvar_system/) 
- Separated Gui/Widget component view creation from the core engine to the editor
- Changed the FPS Clock to use glfw's time instead of std::chrono. ( NOTE: fps seems to vary more with this i.e. with a target of 60 it goes 59.9-60.1 a lot more than before)

### Important notes
#### Physics system
I tried to get some form of interpolation working, but what I could come up with at the time without breaking other implementation did not work. Running the fixed and variable fps at the same target does deal with the jitter but obviously isn't a good or final solution. 
#### Transform System
No hierarchy. It had started to be implemented but wasn't finished. Full implementation would require some restructuring and likely unifying with the renderer's nodes

- - -
##### Note
visual studio, vcpkg and the vulkan 1.4 sdk (1.4.313.2 is what I used) are needed to run/build the project
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
   		- Where I got some basic textures
	- ### [imgui_entt_entity_editor](https://github.com/Green-Sky/imgui_entt_entity_editor)
	  	- Basis of a lot of the editor through concept. Not directly used but got a lot of core concepts
   	- ### [Vulkan Tutorial](https://vulkan-tutorial.com/)
   	  	- Learning Vulkan and more related concepts. Used to help abstract core Vulkan concepts and items
   	- ### [vkguide](https://vkguide.dev/)
   	  	- Same as above. Mostly used as basis for the renderer's architecture
   	- ### [Khronos Vulkan Tutorial](https://docs.vulkan.org/tutorial/latest/00_Introduction.html)
   	  	- Used in 2025 as a more modern vulkan reference specifically for using 1.4 as baseline
   	- ### [cpp-gamedev.github.io/learn-vulkan](https://cpp-gamedev.github.io/learn-vulkan/index.html)
   		- Similar to the previous link being used as reference for some vulkan systems
  
