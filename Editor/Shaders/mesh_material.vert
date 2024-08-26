#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_buffer_reference : require

#include "Shaders/input_structures.glsl"

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout(location = 3) out vec3 lightDir;
layout(location = 4) out vec3 eyeDir;
struct Vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants
{
	mat4 render_matrix;
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	float scaleX = length(PushConstants.render_matrix[0].xyz);
	float scaleY = length(PushConstants.render_matrix[1].xyz);
	float scaleZ = length(PushConstants.render_matrix[2].xyz);
	float uniformScale = (scaleX + scaleY + scaleZ) / 3.0;
	outUV = vec2(v.uv_x, v.uv_y);
	
	mat3 normalMatrix = mat3(transpose(inverse(mat3(PushConstants.render_matrix))));
	outNormal = normalize( normalMatrix * v.normal);

	vec3 vertPos = vec3(sceneData.view * PushConstants.render_matrix * vec4(v.position, 1.0f));
	vec3 vertDir = normalize(vertPos);
	eyeDir = -vertDir;
	vec3 lightPos = sceneData.sunlightDirection.xyz*1000.0f;
	//lightPos.xyz - vertPos.xyz
	lightDir = normalize(lightPos-vertPos );



	gl_Position =  sceneData.viewproj * PushConstants.render_matrix *vec4(v.position, 1.0f);
	//outNormal = (PushConstants.render_matrix * vec4(v.normal, 0.f)).xyz;
	outColor = v.color.xyz * materialData.colorFactors.xyz;	


}
