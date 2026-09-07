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
    float k = (roughness * roughness) / 2;
    
    float g1 = G_SchlickGGX(NdotV, k);
    float g2 = G_SchlickGGX(NdotL, k);

    return g1 * g2;
}
float GSF_Smith_IBL(float NdotL, float NdotV, float roughness)
{
    float r = roughness + 1;
    float k = (r * r) / 8;
    
    float g1 = G_SchlickGGX(NdotV, k);
    float g2 = G_SchlickGGX(NdotL, k);

    return g1 * g2;
}

float3 SchlickFresnel(float3 BaseColor, float metalness)
{
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), BaseColor, metalness);
    return F0;
}

float3 FF_Schlick(float3 F0, float VdotH)
{
    return F0 + (1.f - F0) * pow(2, (- 5.55473 * VdotH - 6.98316) * VdotH);
}

float3 EquirectUVToDirection(float2 uv)
{
    float longitude     = (uv.x - 0.5f) * 2.0f * PI;
    float latitude      = (0.5f - uv.y) * PI;
    float cosLatitude   = cos(latitude);
    float3 dir;

    dir.x   = cosLatitude * cos(longitude);
    dir.y   = sin(latitude);
    dir.z   = cosLatitude * sin(longitude);

    return normalize(dir);
}

float2 DirectionToEquirectUV(float3 dir)
{
    dir = normalize(dir);

    float2 uv;
    uv.x = atan2(dir.z, dir.x) / (2.0f * PI) + 0.5f;
    uv.y = 0.5f - asin(clamp(dir.y, -1.0f, 1.0f)) / PI;

    return uv;
}

float RadicalInverseVdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) |
           ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) |
           ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) |
           ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) |
           ((bits & 0xFF00FF00u) >> 8u);

    return float(bits) * 2.3283064365386963e-10f;
}

float2 Hammersley(uint i, uint sampleCount)
{
    return float2(
        float(i) / float(sampleCount),
        RadicalInverseVdC(i)
    );
}

//z가 +인 반구공간
float3 CosineSampleHemisphere(float2 Xi)
{
    float phi = 2.0f * PI * Xi.x;

    float sinTheta = sqrt(Xi.y);
    float cosTheta = sqrt(1.0f - Xi.y);

    return float3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        cosTheta
    );
}
const uint DIFFUSE_IBL_SAMPLE_COUNT = 1024u;
float3 Diffuse_IBL_Sample(float3 vNormal, texture2D IBLTexture)
{
    float3 vUp = abs(vNormal.y) < 0.999f ? float3(0.0f, 1.0f, 0.0f) : float3(1.0f, 0.0f, 0.0f);
    float3 vTangent = normalize(cross(vUp, vNormal));
    float3 vBitangent = cross(vNormal, vTangent);
    
    float3 vSum = 0.0f;
    for ( uint i = 0u; i < DIFFUSE_IBL_SAMPLE_COUNT; ++i)
    {
        float3 vHemiSphereSample = CosineSampleHemisphere(Hammersley(i, DIFFUSE_IBL_SAMPLE_COUNT));
        float3 vLight = normalize(
            vHemiSphereSample.x * vTangent +
            vHemiSphereSample.y * vBitangent +
            vHemiSphereSample.z * vNormal.xyz
        );
        vSum += IBLTexture.SampleLevel(DefaultSampler, DirectionToEquirectUV(vLight), 0).rgb;
    }
    return PI * vSum / float(DIFFUSE_IBL_SAMPLE_COUNT);
}

float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 vNormal)
{
    float a = Roughness * Roughness;
    float Phi = 2 * PI * Xi.x;
    float CosTheta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
    float SinTheta = sqrt(1 - CosTheta * CosTheta);
    float3 H;
    
    H.x = SinTheta * cos(Phi);
    H.y = SinTheta * sin(Phi);
    H.z = CosTheta;
    
    float3 vUp          = abs(vNormal.y) < 0.999f ? float3(0.0f, 1.0f, 0.0f) : float3(1.0f, 0.0f, 0.0f);
    float3 vTangent     = normalize(cross(vUp, vNormal));
    float3 vBitangent   = cross(vNormal, vTangent);
    
    return vTangent * H.x + vBitangent * H.y + vNormal * H.z;
}

const uint SPECULAR_IBL_SAMPLE_COUNT = 1024u;
float3 SpecularIBL(float3 F0, float Roughness, float3 vNormal, float3 vView, texture2D IBLTexture)
{
    float3 SpecularLighting = 0;
    for (uint i = 0; i < SPECULAR_IBL_SAMPLE_COUNT; i++)
    {
        float2 Xi = Hammersley(i, SPECULAR_IBL_SAMPLE_COUNT);
        float3 H = ImportanceSampleGGX(Xi, Roughness, vNormal);
        float3 L = 2 * dot(vView, H) * H - vView;
        
        float NoV = saturate(dot(vNormal, vView));
        float NoL = saturate(dot(vNormal, L));
        float NoH = saturate(dot(vNormal, H));
        float VoH = saturate(dot(vView, H));
        if (NoL > 0)
        {
            float3 SampleColor = IBLTexture.SampleLevel(DefaultSampler, DirectionToEquirectUV(L), 0).rgb;
            
            float G     = GSF_Smith(NoL, NoV, Roughness);
            
            float Fc    = pow(1 - VoH, 5);
            float3 F    = (1 - Fc) * F0 + Fc;
            
            SpecularLighting += SampleColor * F * G * VoH / max(NoH * NoV, 0.0001f); 
        } 
    }
    return SpecularLighting / SPECULAR_IBL_SAMPLE_COUNT;
}

const uint PREFILTER_SAMPLE_COUNT = 1024u;
float3 PrefilterEnvMap(float rougness, float3 vReflect, texture2D IBLTexture)
{
    float3 vNormal              = vReflect;
    float3 vView                = vReflect;
    float3 vPrefilteredColor    = 0;
    float  fTotalWeight         = 0.f;
    
    for (uint i = 0; i < PREFILTER_SAMPLE_COUNT; i++) {
        float2 Xi = Hammersley(i, PREFILTER_SAMPLE_COUNT);
        float3 vHalf = ImportanceSampleGGX(Xi, rougness, vNormal);
        float3 vLight = 2 * dot(vView, vHalf) * vHalf - vView;
        float  NoL = saturate(dot(vNormal, vLight));
        if (NoL > 0) {
            vPrefilteredColor += 
            IBLTexture.SampleLevel(DefaultSampler,DirectionToEquirectUV(vLight),0).rgb*NoL;
            fTotalWeight += NoL;
        }
    }
    return vPrefilteredColor / max(fTotalWeight, 0.0001f);
}

const uint BRDF_LUT_SAMPLE_COUNT = 1024u;
float2 IntegrateBRDF(float roughness, float NoV)
{
    float3 vView = float3(sqrt(1.0f - NoV * NoV), 0, NoV);
    float A = 0; 
    float B = 0;
    float3 vNormal = float3(0.0f, 0.0f, 1.0f);
    
    for (uint i = 0; i < BRDF_LUT_SAMPLE_COUNT; i++) {
        float2 Xi = Hammersley(i, BRDF_LUT_SAMPLE_COUNT);
        float3 vHalf = ImportanceSampleGGX(Xi, roughness, vNormal);
        float3 vLight = 2 * dot(vView, vHalf) * vHalf - vView;
        float NoL = saturate(vLight.z);
        float NoH = saturate(vHalf.z);
        float VoH = saturate(dot(vView, vHalf));
        if (NoL > 0) {
            float G = GSF_Smith(NoL, NoV, roughness);
            float G_Vis = G * VoH / max(NoH * NoV, 0.0001f);
            float Fc = pow(1 - VoH, 5);
            A += (1 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    return float2(A, B) / BRDF_LUT_SAMPLE_COUNT;
}

float4 UVToRRoughness(float2 vTexcoord)
{
    float4 vResult;
    
    float scaledV = vTexcoord.y * float(8);
    uint layerIndex = min(uint(scaledV), 8 - 1u);
    float localV = scaledV - float(layerIndex);
    float2 localUV = float2(vTexcoord.x, saturate(localV));
    
    vResult.xyz = EquirectUVToDirection(localUV);    // R
    vResult.w   = float(layerIndex) / float(8 - 1u); // rougness
    
    return vResult;
}
float3 RRoughnessToValue(float4 vRRoughness, texture2D FilterTexture)
{
    float3 R = normalize(vRRoughness.xyz);
    float roughness = saturate(vRRoughness.w);

    float2 localUV = DirectionToEquirectUV(R);

    // roughness를 [0, 7] 레이어 좌표로 변환
    float layer = roughness * float(8 - 1u);

    uint layer0 = uint(floor(layer));
    uint layer1 = min(layer0 + 1u, 8 - 1u);
    float blend = frac(layer);

    // 각 레이어 내부의 UV
    float2 uv0;
    uv0.x = frac(localUV.x);
    uv0.y = (float(layer0) + saturate(localUV.y))
          / float(8);

    float2 uv1;
    uv1.x = frac(localUV.x);
    uv1.y = (float(layer1) + saturate(localUV.y))
          / float(8);

    float3 color0 =
        FilterTexture.SampleLevel(DefaultSampler, uv0, 0.0f).rgb;

    float3 color1 =
        FilterTexture.SampleLevel(DefaultSampler, uv1, 0.0f).rgb;

    return lerp(color0, color1, blend);
}
#endif