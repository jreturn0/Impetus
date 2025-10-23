#version 460

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_buffer_reference : require

#include "assets/Shaders/input_structures.glsl"

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout(location = 3) out vec3 outWorldPos;
struct Vertex {

	vec3 position;
	float uvX;
	vec3 normal;
	float uvY;
	vec4 color;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{
	mat4 modelMatrix;
	VertexBuffer vertexBuffer;
} pushConstants;

void main() 
{
	Vertex v = pushConstants.vertexBuffer.vertices[gl_VertexIndex];
	
	vec4 position = vec4(v.position, 1.0f);
	gl_Position =  sceneData.viewproj * pushConstants.modelMatrix *position;

	outNormal = (pushConstants.modelMatrix * vec4(v.normal, 0.f)).xyz;
	outColor = v.color.xyz * materialData.colorFactors.xyz;	
	outUV.x = v.uvX;
	outUV.y = v.uvY;
	outWorldPos = (pushConstants.modelMatrix * position).xyz;
}
