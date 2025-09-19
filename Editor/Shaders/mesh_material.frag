#version 450

#extension GL_GOOGLE_include_directive : require
#include "Shaders/input_structures.glsl"
layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;
layout(location = 3) in vec3 lightDir;
layout(location = 4) in vec3 eyeDir;
layout (location = 0) out vec4 outFragColor;

const float PI = 3.14159265358979323846;


void main() 
{
    float lightValue = max(dot(inNormal, sceneData.sunlightDirection.xyz), 0.1f);

    vec3 color = inColor * texture(colorTex,inUV).xyz;
    vec3 ambient = color *  sceneData.ambientColor.xyz;

    outFragColor = vec4(color * lightValue *  sceneData.sunlightColor.w + ambient ,1.0f);
}