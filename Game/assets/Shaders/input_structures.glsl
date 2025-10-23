layout(set = 0, binding = 0) uniform  SceneData{

	mat4 view; //inverse 
	mat4 proj;
	mat4 viewproj;
	vec4 ambientColor;
	vec4 sunlightDirection; //w for distance
	vec4 sunlightColor; //w for sun power
} sceneData;
struct Light {
	vec4 dirPos; //w determines type: 0 = directional, 1 = point
	vec4 colour; //w for power/intensity
	vec4 attenuation; //constant, linear, quadratic, range for point lights
};
layout(set = 0, binding = 1) readonly buffer  LightData{

	Light lights[64];
	uint lightCount;
	vec3 padding; //Padding to align to 16 bytes
} lightData;

layout(set = 1, binding = 0) uniform GLTFMaterialData{

	vec4 colorFactors;
	vec4 metallicRoughnessFactors;
} materialData;

layout(set = 1, binding = 1) uniform sampler2D colorTex;
layout(set = 1, binding = 2) uniform sampler2D metalRoughTex;
