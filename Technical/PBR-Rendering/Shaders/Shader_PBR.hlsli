#ifndef SHADER_PBR_HLSLI
#define SHADER_PBR_HLSLI
#include "Shader_Defines.hlsli"

float NDF_GGX_DISNEY(float roughness, float NdotH)
{
    float a     = roughness * roughness;
    float a2    = a* a;
    float NdotH2 = NdotH * NdotH;
    float d = (a2 - 1) * NdotH2 + 1;
    //d = max(d, 0.0001f);
    return a2 / max(PI * d * d, 0.00001f);
}

float GSF_CookTorrence(float NdotL, float NdotV, float VdotH, float NdotH)
{
    float Gs = min(1.0, min(2 * NdotH * NdotV / VdotH, 2 * NdotH * NdotL / VdotH));

    return Gs;
}

float G_SchlickGGX(float NdotV, float k)
{
    return NdotV / (NdotV * (1 - k) + k);
}

float GSF_Smith(float NdotL, float NdotV, float roughness)
{
    float r = roughness + 1;
    float k = (r * r) / 8;
    
    float g1 = G_SchlickGGX(NdotV, k);
    float g2 = G_SchlickGGX(NdotL, k);

    return g1 * g2;
}

//float SchlickFresnel(float i)
//{
//    float x = saturate(1.0 - i);
//    float2 x2 = x * x;
//    return x2 * x2 * x;
//}

//float3 FF_Schlick(float3 SpecularColor, float LdotH)
//{
//    return SpecularColor + (1 - SpecularColor) * SchlickFresnel(LdotH);
//}

float3 SchlickFresnel(float3 BaseColor, float metalness)
{
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), BaseColor, metalness);
    return F0;
}

float3 FF_Schlick(float3 F0, float VdotH)
{
    return F0 + (1.f - F0) * pow(2, (- 5.55473 * VdotH - 6.98316) * VdotH);
}
#endif