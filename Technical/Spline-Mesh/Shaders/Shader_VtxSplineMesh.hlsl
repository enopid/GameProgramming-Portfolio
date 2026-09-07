#include"Shader_Defines.hlsli"
#include"Shader_Util.hlsli"
#include"Shader_Effect.hlsli"
#include"Shader_Texture.hlsli"


matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_camPosition;
float3 g_vLightDir;
matrix g_matWVP, g_matVP;

float g_fShadowJudge;

//Default Start
uint g_uiUseCustomDiffuse = 0;
float3 g_vDiffuseColor = { 0.f, 0.f, 0.f };
float3 g_vDiffusePlus = { 0.f, 0.f, 0.f };

uint g_uiUseCustomEmissive = 0;
float3 g_vEmissiveColor = { 0.f, 0.f, 0.f };

uint g_uiReject = 0;
float g_fU = 0.f;
float g_fV = 0.f;

uint g_uiDiscardX = 0;
uint g_uiDiscardY = 0;
float2 g_f2DiscardLimit = { 0.f, 0.f };

uint g_uiUseCustomSpecular = 0;
Texture2D g_SpecularTexture;

uint g_uiUseCustomTexture = 0;
Texture2D g_CustomTexture;


float3 g_vNCColor = { 0.f, 0.f, 0.f };
float3 g_vELColor = { 0.f, 0.f, 0.f };
//Default End

float g_fEmissiveIntensity = 1.f;

//spline
cbuffer CB_SPLINE : register(b0)
{
    float4 g_pos    [32];
    float4 g_tanget [32];
};

struct VS_IN
{
    float3 vPosition    : POSITION;
    float3 vNormal      : NORMAL;
    
    float2 vTexcoord0   : TEXCOORD0;
    float2 vTexcoord1   : TEXCOORD1;
    float2 vTexcoord2   : TEXCOORD2;
    float2 vTexcoord3   : TEXCOORD3;
    
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
};
struct VS_OUT
{
    float4 vPosition    : SV_Position;
    
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    
    float2 vTexcoord0   : TEXCOORD0;
    float2 vTexcoord1   : TEXCOORD1;
    float2 vTexcoord2   : TEXCOORD2;
    float2 vTexcoord3   : TEXCOORD3;
    
    float4 vProjPos     : TEXCOORD4;
    float3 vWorld : TEXCOORD5;
};
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord0   = In.vTexcoord0;
    Out.vTexcoord1   = In.vTexcoord1;
    Out.vTexcoord2   = In.vTexcoord2;
    Out.vTexcoord3   = In.vTexcoord3;
    Out.vProjPos = Out.vPosition;
    Out.vTangent    = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal   = normalize(mul(float4(In.vBinormal,0.f), g_WorldMatrix)).xyz;
    Out.vWorld = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_Position;
    float2 vTexcoord0 : TEXCOORD0;
};

VS_OUT_SHADOW VS_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out;
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_matWVP);
    Out.vTexcoord0 = In.vTexcoord0;
    return Out;
}

struct PS_IN
{
    float4 vPosition    : SV_Position;
    
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    
    
    float2 vTexcoord0   : TEXCOORD0;
    float2 vTexcoord1   : TEXCOORD1;
    float2 vTexcoord2   : TEXCOORD2;
    float2 vTexcoord3   : TEXCOORD3;
    
    float4 vProjPos     : TEXCOORD4;
    float3 vWorld       : TEXCOORD5;
};
struct PS_OUT
{
    float4 vDiffuseB : SV_Target0;
    float4 vNormal : SV_Target1;
    float1 vDepth : SV_Target2;
    float3 vEmissive : SV_Target3;
    float3 vELSR : SV_Target4; //EdgeLine, Specular, Roughness
};
PS_OUT PS_MAIN(PS_IN In, bool IsFrontFace : SV_IsFrontFace)
{
    if (g_uiReject)
        discard;
    
    PS_OUT Out;
    
// Diffuse
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_BC))
    {
        float4 f4Diffuse = g_HBKTextures[TEXTURE_TYPE_BC].Sample(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV));
        if (g_uiUseCustomTexture)
            f4Diffuse = g_CustomTexture.Sample(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV));
        if (f4Diffuse.a < 0.1f)
            discard;
        Out.vDiffuseB.rgb = f4Diffuse.rgb + g_vDiffusePlus;
    }
    else
        Out.vDiffuseB = 0.f;
    
    // Shadow
    float3 vShadow;
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_SBC))
    {
        vShadow = g_HBKTextures[TEXTURE_TYPE_SBC].Sample(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV)).rgb;
        Out.vNormal.a = 1.f;
    }
    else
    {
        vShadow = Out.vDiffuseB.rgb;
        Out.vNormal.a = 0.f;
    }
    
    // Normal and Shade
    float3 vNormal, vTangent, vBinormal;
    vNormal = In.vNormal;
    vTangent = In.vTangent;
    vBinormal = In.vBinormal;
    if (!IsFrontFace)
    {
        vNormal *= -1.0f;
        vTangent *= -1.0f;
        vBinormal *= -1.0f;
    }
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_N))
    {
        Out.vNormal.xyz = mul(
            g_HBKTextures[TEXTURE_TYPE_N].Sample(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV)).xyz * 2.f - 1.f,
            float3x3(vTangent, vBinormal * -1.f, vNormal)
        );
    }
    else
        Out.vNormal.xyz = In.vNormal;
    if (Out.vNormal.a > 0.5f && saturate(dot(normalize(g_vLightDir) * -1.f, Out.vNormal.xyz)) < g_fShadowJudge)
        Out.vDiffuseB.rgb = vShadow;
    
    if (g_uiUseCustomDiffuse)
    {
        Out.vDiffuseB.rgb = g_vDiffuseColor;
        Out.vNormal.a = 0.1f;
    }
    
    // Emissive
    if (g_uiUseCustomEmissive)
    {
        Out.vEmissive.rgb = g_vEmissiveColor;
        Out.vDiffuseB.a = 1.f;
    }
    else if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_E))
    {
        Out.vEmissive = g_HBKTextures[TEXTURE_TYPE_E].Sample(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV)).xyz * g_vEmissiveColor;
        if (g_uiUseCustomTexture)
            Out.vEmissive = g_CustomTexture.Sample(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV)).xyz * g_vEmissiveColor;
        Out.vDiffuseB.a = 1.f; // No Blur for them with EmissiveMask
    }
    else
    {
        Out.vEmissive.rgb = g_vEmissiveColor;
        Out.vDiffuseB.a = 0.f;
    }
    if (g_uiDiscardX)
    {
        if (g_f2DiscardLimit.x <= g_f2DiscardLimit.y)
        {
            if (In.vTexcoord0.x <= g_f2DiscardLimit.x || g_f2DiscardLimit.y <= In.vTexcoord0.x)
                Out.vEmissive = 0.f;
        }
        else if (g_f2DiscardLimit.x < In.vTexcoord0.x || In.vTexcoord0.x < g_f2DiscardLimit.y)
            Out.vEmissive = 0.f;
    }
    if (g_uiDiscardY)
    {
        if (g_f2DiscardLimit.x <= g_f2DiscardLimit.y)
        {
            if (In.vTexcoord0.y <= g_f2DiscardLimit.x || g_f2DiscardLimit.y <= In.vTexcoord0.y)
                Out.vEmissive = 0.f;
        }
        else if (g_f2DiscardLimit.x < In.vTexcoord0.y || In.vTexcoord0.y < g_f2DiscardLimit.y)
            Out.vEmissive = 0.f;
    }
    
    // Mask
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_EL))
        Out.vDiffuseB.rgb = lerp(float3(0.f, 0.f, 0.f), Out.vDiffuseB.rgb, g_HBKTextures[TEXTURE_TYPE_EL].Sample(DefaultSampler, In.vTexcoord1).x);
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_NC))
        Out.vDiffuseB.rgb = lerp(float3(0.f, 0.f, 0.f), Out.vDiffuseB.rgb, g_HBKTextures[TEXTURE_TYPE_NC].Sample(DefaultSampler, In.vTexcoord2).x);
    
    // Roughness
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_MBR) && In.vTexcoord0.y <= 0.5f)
        Out.vELSR.b = g_HBKTextures[TEXTURE_TYPE_MBR].Sample(ClampSampler, In.vTexcoord0 + float2(g_fU, g_fV)).x;
    else if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_R) && In.vTexcoord0.y <= 0.5f)
        Out.vELSR.b = g_HBKTextures[TEXTURE_TYPE_R].Sample(ClampSampler, In.vTexcoord0 + float2(g_fU, g_fV)).x;
    else
        Out.vELSR.b = 0.f;
    
    // EdgeLine
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_EL))
        Out.vELSR.r = g_HBKTextures[TEXTURE_TYPE_EL].Sample(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV)).x;
    else
        Out.vELSR.r = 1.f;
    
    // Specular
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_SPM))
    {
        float2 f2UV = GetWorldSpaceUV(In.vWorld, In.vNormal);
        float fCam = (g_camPosition.x + g_camPosition.y + g_camPosition.z) * length(g_camPosition) / max(abs(g_camPosition.x) + abs(g_camPosition.y) + abs(g_camPosition.z), EPS);
        float fSpecular = g_HBKTextures[TEXTURE_TYPE_SPM].Sample(DefaultSampler, float2(frac((fCam + f2UV.x + f2UV.y) * 0.025f) * 4.f, 0)).r;
        Out.vELSR.g = step(0.5f, fSpecular);
    }
    else if (g_uiUseCustomSpecular)
    {
        float2 f2UV = GetWorldSpaceUV(In.vWorld, In.vNormal);
        float fCam = (g_camPosition.x + g_camPosition.y + g_camPosition.z) * length(g_camPosition) / max(abs(g_camPosition.x) + abs(g_camPosition.y) + abs(g_camPosition.z), EPS);
        float fSpecular = g_SpecularTexture.Sample(DefaultSampler, float2(frac((fCam + f2UV.x + f2UV.y) * 0.025f) * 4.f, 0)).r;
        Out.vELSR.g = step(0.5f, fSpecular);
    }
        Out.vELSR.g = 0.f;
    
    // Depth
    Out.vDepth.x = In.vProjPos.z / In.vProjPos.w;
    
    return Out;
}

PS_OUT PS_EXHIBITION(PS_IN In, bool IsFrontFace : SV_IsFrontFace)
{
    PS_OUT Out;
    Out.vDepth = 0.f;
    Out.vDiffuseB = 0.f;
    Out.vELSR = 0.f;
    Out.vEmissive = 0.f;
    Out.vNormal = 0.f;
    
// Diffuse
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_BC))
    {
        float4 f4Diffuse = g_HBKTextures[TEXTURE_TYPE_BC].Sample(DefaultSampler, In.vTexcoord0);
        if (f4Diffuse.a < 0.1f)
            discard;
        Out.vDiffuseB.rgb = f4Diffuse.rgb;
    }
    else
        Out.vDiffuseB = 0.f;
    
    // Shadow
    float3 vShadow;
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_SBC))
    {
        vShadow = g_HBKTextures[TEXTURE_TYPE_SBC].Sample(DefaultSampler, In.vTexcoord0).rgb;
        Out.vNormal.a = 1.f;
    }
    else
    {
        vShadow = Out.vDiffuseB.rgb;
        Out.vNormal.a = 0.f;
    }
    
    // Normal and Shade
    float3 vNormal, vTangent, vBinormal;
    vNormal = In.vNormal;
    vTangent = In.vTangent;
    vBinormal = In.vBinormal;
    if (!IsFrontFace)
    {
        vNormal *= -1.0f;
        vTangent *= -1.0f;
        vBinormal *= -1.0f;
    }
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_N))
    {
        Out.vNormal.xyz = mul(
            g_HBKTextures[TEXTURE_TYPE_N].Sample(DefaultSampler, In.vTexcoord0).xyz * 2.f - 1.f,
            float3x3(vTangent, vBinormal * -1.f, vNormal)
        );
    }
    else
        Out.vNormal.xyz = In.vNormal;
    if (Out.vNormal.a > 0.5f && saturate(dot(normalize(g_vLightDir) * -1.f, Out.vNormal.xyz)) < g_fShadowJudge)
        Out.vDiffuseB.rgb = vShadow;
    
    // Emissive
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_E))
    {
        Out.vEmissive = g_HBKTextures[TEXTURE_TYPE_E].Sample(DefaultSampler, In.vTexcoord0).xyz * g_vEmissiveColor;
        Out.vDiffuseB.a = 1.f; // No Blur for them with EmissiveMask
    }
    else
    {
        Out.vEmissive.rgb = g_vEmissiveColor;
        Out.vDiffuseB.a = 0.f;
    }
    
    // Mask
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_NC))
        Out.vDiffuseB.rgb = lerp(float3(0.f, 0.f, 0.f), Out.vDiffuseB.rgb, g_HBKTextures[TEXTURE_TYPE_NC].Sample(DefaultSampler, In.vTexcoord1).x);
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_EL))
        Out.vDiffuseB.rgb = lerp(float3(0.f, 0.f, 0.f), Out.vDiffuseB.rgb, g_HBKTextures[TEXTURE_TYPE_EL].Sample(DefaultSampler, In.vTexcoord1).x);
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_SPM))
        Out.vDiffuseB.rgb = lerp(Out.vDiffuseB.rgb, float3(1.f, 1.f, 1.f), g_HBKTextures[TEXTURE_TYPE_SPM].Sample(DefaultSampler, In.vTexcoord2).x * 0.3f);
    
    // Roughness
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_MBR) && In.vTexcoord0.y <= 0.5f)
        Out.vELSR.b = g_HBKTextures[TEXTURE_TYPE_MBR].Sample(DefaultSampler, In.vTexcoord0).x;
    else if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_R) && In.vTexcoord0.y <= 0.5f)
        Out.vELSR.b = g_HBKTextures[TEXTURE_TYPE_R].Sample(DefaultSampler, In.vTexcoord0).x;
    else
        Out.vELSR.b = 0.f;
    
    // EdgeLine
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_EL))
        Out.vELSR.r = g_HBKTextures[TEXTURE_TYPE_EL].Sample(DefaultSampler, In.vTexcoord0).x;
    else
        Out.vELSR.r = 1.f;
    
    // Depth
    Out.vDepth.x = In.vProjPos.z / In.vProjPos.w;
    
    return Out;
}

struct PS_IN_SHADOW
{
    float4 vPosition : SV_Position;
    float2 vTexcoord0 : TEXCOORD0;
};
struct PS_OUT_SHADOW
{
    float1 vDepth : SV_Target0;
};

PS_OUT_SHADOW PS_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out;
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_BC))
    {
        float4 f4Diffuse = g_HBKTextures[TEXTURE_TYPE_BC].Sample(DefaultSampler, In.vTexcoord0);
        if (f4Diffuse.a < 0.1f)
            discard;
    }
    else
        discard;
    Out.vDepth = In.vPosition.z;
    return Out;
}

struct PS_OUT_GLASS
{
    float1 vGlass : SV_Target0;
    float3 vELSR : SV_Target1;
};

PS_OUT_GLASS PS_GLASS(PS_IN In)
{
    PS_OUT_GLASS Out;
    Out.vGlass.x = 1.f;
    
    // EdgeLine
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_EL))
        Out.vELSR.r = g_HBKTextures[TEXTURE_TYPE_EL].Sample(DefaultSampler, In.vTexcoord0).x;
    else
        Out.vELSR.r = 1.f;
   
    // Specular
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_SPM))
    {
        float2 f2UV = GetWorldSpaceUV(In.vWorld, In.vNormal);
        float fCam = (g_camPosition.x + g_camPosition.y + g_camPosition.z) * length(g_camPosition) / max(abs(g_camPosition.x) + abs(g_camPosition.y) + abs(g_camPosition.z), EPS);
        float fSpecular = g_HBKTextures[TEXTURE_TYPE_SPM].Sample(ClampSampler, float2(frac((fCam + f2UV.x + f2UV.y) * 0.025f) * 4.f, 0)).r;
        Out.vELSR.g = step(0.5f, fSpecular);
    }
    else
        Out.vELSR.g = 0.f;
    
    // Roughness
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_MBR) && In.vTexcoord0.y <= 0.5f)
        Out.vELSR.b = g_HBKTextures[TEXTURE_TYPE_MBR].Sample(DefaultSampler, In.vTexcoord0).x;
    else if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_R) && In.vTexcoord0.y <= 0.5f)
        Out.vELSR.b = g_HBKTextures[TEXTURE_TYPE_R].Sample(DefaultSampler, In.vTexcoord0).x;
    else
        Out.vELSR.b = 0.f;
    
    return Out;
}

struct PS_OUT_UNLIT
{
    float4 vColor : SV_Target0;
};

PS_OUT_UNLIT PS_BARRIER(PS_IN In)
{
    PS_OUT_UNLIT Out;
  
    float3 vColor0 = { 0.01f, 0.23f, 0.72f };
    float3 vColor1 = { 0.00f, 0.08f, 0.63f };
    float3 vColor2 = { 0.46f, 0.57f, 0.98f };
    
    float2 vUV = In.vTexcoord0;
    float2 vUVScale = { 8.f, 8.f };
    vUV *= vUVScale;
    
    float w0 = g_HBKTextures[TEXTURE_TYPE_BC]. Sample(DefaultSampler, vUV);
    float w2 = g_HBKTextures[TEXTURE_TYPE_SPM].Sample(DefaultSampler, vUV);
    float w1 = 1.f - w0 - w2;
    
    Out.vColor.rgb =
    vColor0 * w0 +
    vColor1 * w1 +
    vColor2 * w2;
    
    Out.vColor.a = 0.516f;
    
    
    
    return Out;
}

PS_OUT_UNLIT PS_ZSHIELD(PS_IN In)
{
    PS_OUT_UNLIT Out;
  
    float3 vColor0 = { 1.00f, 0.43f, 0.21f };
    float3 vColor1 = { 1.00f, 0.02f, 0.00f };
    float3 vColor2 = { 0.48f, 0.01f, 0.00f };
    
    float2 vUVSpeed = { 0.f, 0.1f };
    float2 vUVScale = { 1.f, 3.f };
    
    float2 vUV = In.vTexcoord0;
    vUV *= vUVScale;
    vUV += vUVSpeed * g_fU;
    
    float w0 = g_HBKTextures[TEXTURE_TYPE_BC].Sample(DefaultSampler,    vUV);
    float w1 = g_HBKTextures[TEXTURE_TYPE_SPM].Sample(DefaultSampler,   vUV);
    Out.vColor.rgb = lerp(vColor0, vColor1, w0);
    Out.vColor.rgb = lerp(Out.vColor.rgb, vColor2, w1);
    Out.vColor.a = 1.f;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }
    pass ShadowPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SHADOW();
    }
    pass DefaultBlendPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Character, StencilEffect);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass MaskPass
    {
        SetRasterizerState(RS_Mask);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass FrontPass
    {
        SetRasterizerState(RS_Front);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass ExhibitionPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EXHIBITION();
    }
    pass GlassPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_NoWrite, 0);
        SetBlendState(BS_ColorMax, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GLASS();
    }
    pass BarrierPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_NoWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BARRIER();
    }
    pass ZShieldPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect_DepthWrite, StencilEffect);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ZSHIELD();
    }
}

//#include "Shader_Defines.hlsli"
//#include "Shader_Util.hlsli"
//#include "Shader_Effect.hlsli"

//matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//float4      g_camPosition;

//bool        g_bUseMask;
//int         g_iMaskType;
//texture2D   g_MetalnessTexture;
//bool        g_bUseSSS;
//texture2D   g_ShininessTexture;

//bool        g_bUseAlphaTest;
//bool        g_bUseDiffuseAlpha;

//bool        g_bUseDiffuse;
//texture2D   g_DiffuseTexture;
//bool        g_bUseOpacitiy;
//texture2D   g_OpacityTexture;
//bool        g_bUseNormal;
//texture2D   g_NormalTexture;

//bool        g_bUseEmisive;
//texture2D   g_EmisiveTexture;

//float4      g_vNoiseOffset = { 0.f,0.f,0.f,0.f };

//bool        g_bIsTwoSided;
//bool        g_bIsBombInteract;
//float       g_fTransparentCoeff = 1.f;
//float       g_fDiffuseBoost     = 1.f;
//float       g_fEmissiveBoost     = 1.f;

//float       g_fMetalBoost = 0.f;
//texture2D g_DepthTexture;

//float3 GetMRAO(float3 vColor, float2 vTexcoord) {
//    float3 vOutColor;
//    vOutColor = vColor;

//    vOutColor.rgb *= g_MetalnessTexture.Sample(DefaultSampler, vTexcoord).b;

//    return vOutColor;
//}

//struct VS_IN
//{
//    float3 vPosition    : POSITION;
//    float3 vNormal      : NORMAL;
//    float2 vTexcoord    : TEXCOORD0;
//    float3 vTangent     : TANGENT;
//    float3 vBinormal    : BINORMAL;
//};

//struct VS_OUT
//{
//    float4 vPosition    : SV_POSITION;
    
//    float4 vNormal      : NORMAL;
//    float4 vTangent     : TANGENT;
//    float4 vBinormal    : BINORMAL;

//    float2 vTexcoord    : TEXCOORD0;
//    float4 vWorldPos    : TEXCOORD1;
//    float4 vProjPos     : TEXCOORD2;
//};

//VS_OUT VS_MAIN(VS_IN In)
//{
//    VS_OUT Out;
    
//    float4x4 matWV, matWVP;
    
//    matWV   = mul(g_WorldMatrix, g_ViewMatrix);
//    matWVP  = mul(matWV, g_ProjMatrix);
    
//    Out.vPosition   = mul(float4(In.vPosition, 1.f),    matWVP);
//    //float3x3 normalMat = (float3x3)transpose(inverse(g_WorldMatrix));
//    //Out.vNormal     = vector(normalize(mul(In.vNormal,      normalMat)), 0.f);
//    //Out.vTangent    = vector(normalize(mul(In.vTangent,     normalMat)), 0.f);
//    //Out.vBinormal   = vector(normalize(mul(In.vBinormal,    normalMat)), 0.f);
    
//    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
//    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix));
//    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix));

//    Out.vTexcoord   = In.vTexcoord;
//    Out.vWorldPos   = mul(float4(In.vPosition, 1.f),            g_WorldMatrix);
//    Out.vProjPos    = Out.vPosition;

//    return Out;
//}

//struct PS_IN
//{
//    float4 vPosition    : SV_POSITION;
    
//    float4 vNormal      : NORMAL;
//    float4 vTangent     : TANGENT;
//    float4 vBinormal    : BINORMAL;

//    float2 vTexcoord    : TEXCOORD0;
//    float4 vWorldPos    : TEXCOORD1;
//    float4 vProjPos     : TEXCOORD2;
//};

//struct PS_DEFFERED_OUT
//{
//    float4 vDiffuse : SV_TARGET0;
//    float4 vMRAOS   : SV_TARGET1;
//    float4 vEmissive: SV_TARGET2;
//    float4 vNormal  : SV_TARGET3;
//    float4 vDepth   : SV_TARGET4;
//};

//struct PS_OUT
//{
//    float4 vColor : SV_TARGET0;
//};

//PS_DEFFERED_OUT PS_MAIN(PS_IN In, bool isFrontFace : SV_IsFrontFace)
//{
//    PS_DEFFERED_OUT Out;

//    vector vMtrlDiffuse     = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
//    vector vMtrlMRAOS       = { 0.f, .5f, 1.f, 0.f };
//    vector vMtrlEmissive    = { 1.f, 1.f, 1.f, 0.f };
//    vector vMtrlNormal      = vector(In.vNormal.xyz, 0.f);

//    if (!isFrontFace)       vMtrlNormal *= -1.f;
//    if (g_bIsTwoSided)      { vMtrlNormal.w = 1.f; };
    

//    if (g_bUseEmisive)      vMtrlEmissive = vector(g_EmisiveTexture.Sample(DefaultSampler, In.vTexcoord).rgb, 1.f);
//    if (g_bUseOpacitiy) {
//        vector vAlphaColor = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord);
//        if (g_bUseDiffuseAlpha) vMtrlDiffuse.a = vAlphaColor.a;
//        else                    vMtrlDiffuse.a = vAlphaColor.r;
//    }
//    if (g_bUseAlphaTest && vMtrlDiffuse.a <= 0.1f) discard;

//    if (g_bUseSSS)      vMtrlMRAOS.w = g_ShininessTexture.Sample(DefaultSampler, In.vTexcoord).r;
//    if (g_bUseNormal) {
//        vector normalTS = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
//        normalTS.rgb = normalTS.rgb * 2.f - 1.f;
//        normalTS.z      = sqrt(saturate(1.0f - dot(normalTS.xy, normalTS.xy)));

//        vMtrlNormal.rgb =
//            normalTS.x  * In.vTangent.rgb +
//            -normalTS.y * In.vBinormal.rgb +
//            normalTS.z  * vMtrlNormal.rgb;

//        vMtrlNormal.rgb = normalize(vMtrlNormal.xyz);
//    }

//    if (g_bUseMask) { 
//        switch (g_iMaskType) 
//        {
//        case 0: //None
//            break;
//        case 1: //MRAO
//            vMtrlMRAOS.r    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).r;
//            vMtrlMRAOS.g    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).g;
//            vMtrlMRAOS.b    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).b;
//            break;
//        case 2: //ERAO
//            vMtrlEmissive.w = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).r;
//            vMtrlMRAOS.g    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).g;
//            vMtrlMRAOS.b    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).b;
//            break;
//        case 3: //HRAO
//            vMtrlMRAOS.g    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).g;
//            vMtrlMRAOS.b    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).b;
//            break;
//        case 4: //AORM
//            vMtrlMRAOS.r    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).b;
//            vMtrlMRAOS.g    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).g;
//            vMtrlMRAOS.b    = g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).r;
//            break;
//        default:
//            break;
//        }
//    }
//    vMtrlEmissive.rgb *= vMtrlEmissive.w;
//    vMtrlEmissive.w = 1.f;

//    Out.vDiffuse    = vMtrlDiffuse;
//    Out.vMRAOS      = vMtrlMRAOS;
//    Out.vEmissive   = vMtrlEmissive;
//    Out.vNormal     = vector(vMtrlNormal.xyz * 0.5f + 0.5f, vMtrlNormal.w);
//    Out.vDepth      = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 5000.f, 0.f, 0.f);

//    if (g_bIsBombInteract) Out.vDepth.z = 1.f;

//    Out.vDiffuse.rgb    *= g_fDiffuseBoost;
//    Out.vEmissive.rgb   *= g_fEmissiveBoost;
//    Out.vDiffuse = Dissolve(Out.vDiffuse, In.vTexcoord);
//    Out.vDiffuse = Overload(Out.vDiffuse, In.vTexcoord);

//    return Out;
//}

//PS_OUT PS_HAIR(PS_IN In)
//{
//    PS_OUT Out;

//    vector vColor = { .1f,.1f,.1f,1.f };

//    if (g_bUseOpacitiy) {
//        vector vAlphaColor = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord);
//        if (g_bUseDiffuseAlpha)
//            vColor.a = vAlphaColor.a;
//        else
//            vColor.a = vAlphaColor.r;
//    }

//    if (g_bUseDiffuse)
//        vColor.rgb = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
//    if (g_bUseAlphaTest && vColor.a <= 0.01f) discard;
    
//    Out.vColor = vColor;

//    Out.vColor = Dissolve(Out.vColor, In.vTexcoord);

    
//    return Out;
//}

//PS_OUT PS_EYE(PS_IN In)
//{
//    PS_OUT Out;

//    vector vColor = { .1f,.1f,.1f,1.f };

//    float2 irisUV = (In.vTexcoord - 0.5) * 3.5;// +0.5;
//    float r2 = dot(irisUV, irisUV);
//    float irisMask = smoothstep(0.25, 0.23, r2);
//    irisUV += 0.5f;

//    float4 vIrisColor   = g_DiffuseTexture.Sample   (DefaultSampler, irisUV);
//    float4 vScleraColor = g_ShininessTexture.Sample(DefaultSampler, In.vTexcoord);
//    vColor.rgb = lerp(vScleraColor.rgb, vIrisColor.rgb, irisMask);

//    Out.vColor = vColor;

//    return Out;
//}

//PS_OUT PS_Prop(PS_IN In)
//{
//    PS_OUT Out;

//    vector vColor = { .1f,.1f,.1f,1.f };

//    if (g_bUseOpacitiy) {
//        vector vAlphaColor = g_OpacityTexture.Sample(DefaultSampler, In.vTexcoord);
//        if (g_bUseDiffuseAlpha)
//            vColor.a = vAlphaColor.a;
//        else
//            vColor.a = vAlphaColor.r;
//    }

//    if (g_bUseDiffuse)
//        vColor.rgb = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord).rgb;

//    if (g_bUseEmisive)
//        vColor.rgb += g_EmisiveTexture.Sample(DefaultSampler, In.vTexcoord).rgb;

//    if (g_bUseAlphaTest && vColor.a <= 0.1f) discard;

//    if (g_bUseMask) {
//        if (g_iMaskType == 4)
//            vColor.rgb *= g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).r;
//        else
//            vColor.rgb *= g_MetalnessTexture.Sample(DefaultSampler, In.vTexcoord).b;
//    }

//    Out.vColor = vColor;

//    Out.vColor = Dissolve(Out.vColor, In.vTexcoord);




//    return Out;
//}

//PS_OUT PS_Corrupt(PS_IN In)
//{
//    PS_OUT Out;
//    Out.vColor = Corrupttion(In.vNormal, In.vWorldPos, g_camPosition, g_DiffuseTexture);

//    Out.vColor = Dissolve(Out.vColor, In.vTexcoord);

//    return Out;
//}

//PS_OUT PS_Staff(PS_IN In)
//{
//    PS_OUT Out;

//    vector vColor = { 1.0f,.8f,.8f,1.f };

//    vector vShapeColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

//    Out.vColor.a = lerp(vShapeColor.r, saturate(vShapeColor.g + vShapeColor.b), 0.1f);
//    Out.vColor.rgb = vColor.rgb;

//    return Out;
//}

//PS_OUT PS_Energy(PS_IN In)
//{
//    PS_OUT Out;


//    float2 uv = In.vProjPos;
//    uv.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
//    uv.y = In.vProjPos.y / In.vProjPos.w * -0.5f + 0.5f;

//    float fScreenDepth = g_DepthTexture.Sample(DefaultSampler, uv).g * 5000.f;
//    float fTargetDepth = In.vProjPos.w;

//    float fDepthDiff = pow((fScreenDepth - fTargetDepth), 2.2f);

//    vector vColor = { .065f, .309f, .644f,1.f };

//    float fNoiseEdge0   = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + g_vNoiseOffset.rg).r;
//    float fNoise        = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord).g;
//    float fNoiseBase    = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord + g_vNoiseOffset.ba).b;
//    float fNoiseEdge1   = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord).a;

//    float4 vView = normalize(g_camPosition - In.vWorldPos);
//    float fFacing = saturate(abs(dot(vView, In.vNormal)));

//    float fRim = pow(1 - fFacing, 1.8) * 1.2f;

//    fRim = lerp(fRim, fRim + fRim * fNoiseEdge0 * fNoiseEdge1, 1.0f);
//    fRim += fNoiseBase * 0.2f;
//    fRim += saturate(1.f - fDepthDiff);
//    Out.vColor = vColor * 1.1f * max(fRim, 1.f);
    
//    Out.vColor.a = lerp(0.1f, 1.f, pow(fRim, 1.7f));
//    Out.vColor.a    *= g_fTransparentCoeff;
//    Out.vColor.rgb = pow(Out.vColor.rgb, g_fGamma);

//    Out.vColor = Dissolve(Out.vColor, In.vTexcoord);
//    Out.vColor = Overload(Out.vColor, In.vTexcoord);

//    return Out;
//}

//technique11 DefaultTechnique
//{
//    pass DefaultPass
//    {
//        VertexShader = compile vs_5_0 VS_MAIN();
//        GeometryShader = NULL;
//        PixelShader = compile ps_5_0 PS_MAIN();
//    }

//    pass HairPass
//    {
//        VertexShader = compile vs_5_0 VS_MAIN();
//        GeometryShader = NULL;
//        PixelShader = compile ps_5_0 PS_HAIR();
//    }

//    pass EyePass
//    {
//        VertexShader = compile vs_5_0 VS_MAIN();
//        GeometryShader = NULL;
//        PixelShader = compile ps_5_0 PS_EYE();
//    }

//    pass PropPass
//    {
//        VertexShader = compile vs_5_0 VS_MAIN();
//        GeometryShader = NULL;
//        PixelShader = compile ps_5_0 PS_MAIN();
//    }
//    pass CorruptPass
//    {
//        VertexShader    = compile vs_5_0 VS_MAIN();
//        GeometryShader = NULL;
//        PixelShader     = compile ps_5_0 PS_Corrupt();
//    }

//    pass StaffPass
//    {
//        VertexShader = compile vs_5_0 VS_MAIN();
//        GeometryShader = NULL;
//        PixelShader = compile ps_5_0 PS_Staff();
//    }

//    pass EnergyPass
//    {
//        VertexShader = compile vs_5_0 VS_MAIN();
//        GeometryShader = NULL;
//        PixelShader = compile ps_5_0 PS_Energy();
//    }w
//}exhbenemy















