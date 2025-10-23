#version 460

#extension GL_GOOGLE_include_directive : require

#include "assets/Shaders/input_structures.glsl"




layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inWorldPos;

layout (location = 0) out vec4 outFragColor;

const float PI = 3.14159265358979323846;

float distributionGGX (vec3 N, vec3 H, float roughness){
    float a2    = roughness * roughness * roughness * roughness;
    float NdotH = max (dot (N, H), 0.0);
    float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float geometrySchlickGGX (float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith (vec3 N, vec3 V, vec3 L, float roughness){
    return geometrySchlickGGX (max (dot (N, L), 0.0), roughness) * 
           geometrySchlickGGX (max (dot (N, V), 0.0), roughness);
}

vec3 fresnelSchlick (float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow (1.0 - cosTheta, 5.0);
}

// https://www.shadertoy.com/view/4d2XWV by Inigo Quilez
float sphereIntersect(vec3 ro, vec3 rd, vec4 sph) {
    vec3 oc = ro - sph.xyz;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - sph.w*sph.w;
    float h = b*b - c;
    if( h<0.0 ) return -1.0;
    return -b - sqrt( h );
}

float computePointAttenuation(vec4 atten, float dist) {
    // avoid division by zero
    const float EPS = 1e-6;
    float c = atten.x;
    float l = atten.y;
    float q = atten.z;
    float range = atten.w;

    // simple inverse polynomial
    float denom = max(EPS, c + l * dist + q * dist * dist);
    float att = 1.0 / denom;

    // optional soft clamp by range: smoothly fade to zero as distance approaches range
    // if range <= 0.0 then don't apply range clamping
    if (range > 0.0) {
        // use smoothstep to fade from 1.0 -> 0.0 over [range*0.9, range]
        float fadeStart = max(0.0, range * 0.9);
        float fade = 1.0 - smoothstep(fadeStart, range, dist);
        att *= fade;
    }

    return att;
}


vec3 drawLight(
    vec3 N,
    vec3 V,
    vec3 L,
    vec3 F0,
    vec3 baseColor,
    float metallic,
    float roughness,
    vec3 radiance)
{
    float NdotL = max(dot(N, L), 0.0);
    if (NdotL <= 0.0) return vec3(0.0);
    vec3 H = normalize(V + L);

    float NDF = distributionGGX(N, H, roughness);
    float G   = geometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
    vec3 specular     = numerator / denominator;

    return (kD * baseColor / PI + specular) * radiance * NdotL;
}




void main()
{
    vec3 albedoTex = texture(colorTex, inUV).rgb;
    vec3 baseColor = inColor * albedoTex * materialData.colorFactors.rgb;
    baseColor = pow(baseColor, vec3(2.2)); // to linear

    vec4 mr = texture(metalRoughTex, inUV);
    float metallic = mr.r * materialData.metallicRoughnessFactors.x;
    float roughness = mr.g * materialData.metallicRoughnessFactors.y;
    roughness = max(roughness, 0.045); // avoid numerical issues
    float ao = 1.0;
    // float ao = mr.r * materialData.metallicRoughnessFactors.z;

    vec3 N = normalize(inNormal);
    vec3 V = normalize(sceneData.view[3].xyz - inWorldPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, baseColor, metallic);

    // outgoing radiance
    vec3 Lo = vec3(0.0);

    // -- sunlight (use same reusable function) --
    {
        vec3 L = normalize(-sceneData.sunlightDirection.xyz);
        vec3 radiance = sceneData.sunlightColor.xyz * sceneData.sunlightColor.w;
        Lo += drawLight(N, V, L, F0, baseColor, metallic, roughness, radiance);
    }

    // -- other lights --
    for (int i = 0; i < lightData.lightCount; ++i)
    {
        vec3 L;
        vec3 radiance = lightData.lights[i].colour.xyz * lightData.lights[i].colour.w;
        // dirPos.w > 0.5 -> point light (dirPos.xyz is position)
        if (lightData.lights[i].dirPos.w > 0.5) {
            L = normalize(lightData.lights[i].dirPos.xyz - inWorldPos);
            float dist = length(lightData.lights[i].dirPos.xyz - inWorldPos);
            float att = computePointAttenuation(lightData.lights[i].attenuation, dist);
            radiance *= att;
        } else {
            // directional light (dirPos.xyz is direction)
            L = normalize(-lightData.lights[i].dirPos.xyz);
        }


        Lo += drawLight(N, V, L, F0, baseColor, metallic, roughness, radiance);
    }

    vec3 ambient = vec3(0.03) * baseColor * sceneData.ambientColor.xyz * ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping + gamma
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outFragColor = vec4(color, 1.0);
}

// https://learnopengl.com/PBR/Lighting