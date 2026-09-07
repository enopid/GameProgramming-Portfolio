#include "Shader_Defines.hlsli"
#include "Shader_Util.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D   g_SceneTexture;
texture2D   g_DepthTexture;
texture2D   g_NormalTexture;

texture2D   g_DecalTexture;
texture2D   g_DecalMaskTexture;
float4      g_vDecalScale;
bool        g_bUseDecalTexture;
texture2D   g_NoiseTexture;

float4      g_vTintColorR;
float4      g_vTintColorG;
float4      g_vTintColorB;
float4      g_vTintColorA;
float       g_fColorBoost;

matrix      g_DecalWorldMatrixInv;

float4      g_vFogColor;
float       g_fFogStart;
float       g_fFogEnd;
float       g_fFogDensity;
bool        g_bUseDensity;

float4      g_vGodRayPos;
float4      g_vGodRayValue;

#define GodRaySample 64

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float4x4 matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_SSD(PS_IN In)
{
    PS_OUT Out;
    vector  vDepthDesc      = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vWorldPos       = Depth2World(vDepthDesc, 5000.f, In.vTexcoord, g_ViewMatrixInv, g_ProjMatrixInv);
    vector  vDecalLocalPos  = mul(vWorldPos, g_DecalWorldMatrixInv);

    if (abs(vDecalLocalPos.x) > 0.5f || abs(vDecalLocalPos.y) > 0.5f || abs(vDecalLocalPos.z) > 0.5f) discard;

    float2 uv;
    uv.x = vDecalLocalPos.x + 0.5f;
    uv.y = vDecalLocalPos.z + 0.5f;

    vector  vColor  = g_DecalTexture.Sample(DefaultSampler, uv);
    vector  vMask   = g_DecalMaskTexture.Sample(DefaultSampler, uv);
    if (g_bUseDecalTexture) {
        float fScale = length(float3(g_DecalWorldMatrixInv._11, g_DecalWorldMatrixInv._21, g_DecalWorldMatrixInv._31));
        vColor = g_DecalTexture.Sample(DefaultSampler, uv * fScale * 150.f);
        vColor *= vMask;
    }

    vector  vFinalColor = 0.f;
    vFinalColor += vColor.r * g_vTintColorR;
    vFinalColor += vColor.g * g_vTintColorG;
    vFinalColor += vColor.b * g_vTintColorB;
    vFinalColor += vColor.a * g_vTintColorA;

    vFinalColor.rgb *= g_fColorBoost;

    if (vFinalColor.a < 0.01f) discard;

    Out.vColor = vFinalColor;

    return Out;
}

PS_OUT PS_FOG(PS_IN In)
{
    PS_OUT Out;
    
    vector  vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vSceneDesc = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);

    float _fFogFactor;
    if (g_bUseDensity)
        _fFogFactor = 1 - exp(-g_fFogDensity * 0.01f * vDepthDesc.g * 5000.f);
    else
        _fFogFactor = smoothstep(g_fFogStart, g_fFogEnd, vDepthDesc.g * 5000.f);

    vector color = lerp(vSceneDesc, g_vFogColor, _fFogFactor);
    Out.vColor = vector(color.rgb, 1.f);

    return Out;
}

PS_OUT PS_GODRAY(PS_IN In)
{
    PS_OUT Out;

    float IlluminationDecay = 1.0;

    vector  vSceneDesc = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float3 vColor = vSceneDesc.rgb;
    float2 _vScreenLightPos = {0.f,0.f};

    float2 _vDeltaTex   = (In.vTexcoord - _vScreenLightPos);
    _vDeltaTex *= g_vGodRayValue.x / GodRaySample;
    
    float2 uv           = In.vTexcoord;


    for (int i = 0; i < GodRaySample; i++)
    {
        uv -= _vDeltaTex;
        vector  vSample = g_SceneTexture.Sample(DefaultSampler, uv);
        vSample *= IlluminationDecay * g_vGodRayValue.z;
        vColor += vSample;
        IlluminationDecay *= g_vGodRayValue.y;
    }

    Out.vColor.rgb = saturate(vColor * g_vGodRayValue.w);
    Out.vColor.w = 1.f;
    return Out;
}

float fWaveRatio       = 0.f;

float fWaveMaskRatio   = 0.08f;  //6.f
float fWaveMaskEdgeRatio   = 0.04f;  //6.f
float fWavePeriod      = 120.f;
vector vWaveCenter      = {0.f, 0.f, 0.f, 1.f};
vector vWaveColor       = {0.2f, 0.2f, 1.f, 1.f};



PS_OUT PS_WAVE(PS_IN In)
{
    PS_OUT Out;

    vector  vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vSceneDesc = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vWorldPos = Depth2World(vDepthDesc, 5000.f, In.vTexcoord, g_ViewMatrixInv, g_ProjMatrixInv);

    float vDist = length((vWorldPos - vWaveCenter).rgb);
    float fRatio = frac((vDist / fWavePeriod) + fWaveRatio);


    vector  vNoisFactor0 = g_NoiseTexture.Sample(DefaultSampler, In.vTexcoord + fWaveRatio).r;
    vector  vNoisFactor1 = g_NoiseTexture.Sample(DefaultSampler, In.vTexcoord).g;

    float fFactor = (
            smoothstep(0.f, fWaveMaskEdgeRatio, fRatio)
        -   smoothstep(fWaveMaskRatio - fWaveMaskEdgeRatio, fWaveMaskRatio, fRatio)
    ) * vDepthDesc.z * vNoisFactor0 * vNoisFactor1;

    Out.vColor = vSceneDesc;
    Out.vColor.rgb = lerp(Out.vColor.rgb, Out.vColor.rgb * 1.4f + vWaveColor.rgb * 3.2f, fFactor);

    return Out;
}

PS_OUT PS_SSAO(PS_IN In)
{
    PS_OUT Out;

    vector  vDepthDesc  = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    vector  vViewPos = Depth2View(vDepthDesc, 5000.f, In.vTexcoord, g_ProjMatrixInv);

    int KERNEL_SIZE = 16;
    float occlusion = 0;

   /* for (int i = 0; i < KERNEL_SIZE; i++)
    {
        float3 samplePos = pos + TBN * kernel[i] * radius;

        float4 offset = mul(Proj, float4(samplePos, 1.0));
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        float sampleDepth = DepthTex.Sample(sampler, offset.xy);

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(pos.z - sampleDepth));

        if (sampleDepth < samplePos.z)
            occlusion += rangeCheck;
    }

    occlusion = 1.0 - (occlusion / KERNEL_SIZE);*/

    return Out;
}

technique11 DefaultTechnique
{
    pass SSD
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_PreMultiply, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile  vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile   ps_5_0 PS_SSD();
    }
    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile  vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile   ps_5_0 PS_SSAO();
    }
    pass FOG
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile  vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile   ps_5_0 PS_FOG();
    }
    pass GODRAY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile  vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile   ps_5_0 PS_GODRAY();
    }
    pass WAVE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile  vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile   ps_5_0 PS_WAVE();
    }
}