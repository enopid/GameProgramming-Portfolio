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
float g_fMipMap = 0.f;
uint g_uiUseDepthBias = 0;
float3 g_vDepthBias = 0.f;
float g_fDepthBias = 0.f;

uint g_uiDiscardX = 0;
uint g_uiDiscardY = 0;
float2 g_f2DiscardLimit = { 0.f, 0.f };

uint g_uiUseCustomSpecular = 0;
Texture2D g_SpecularTexture;

uint g_uiUseCustomTexture = 0;
Texture2D g_CustomTexture;

float g_fReverseGlass = 0.f;
float g_fPenetration = 0.f;

float3 g_vNCColor = { 0.f, 0.f, 0.f };
float3 g_vELColor = { 0.f, 0.f, 0.f };
//Default End

float g_fEmissiveIntensity = 1.f;

bool g_bUseMC = false;
Texture2D g_MCTexture;

uint g_uiUVNum = 0;
uint g_uiOL = 0;
float g_fOL = 0.f;

//spline
#define NUMSPLINESAMPLE 32

float4 g_vSplinePos     [NUMSPLINESAMPLE];
float4 g_vSplineTanget  [NUMSPLINESAMPLE];
float4 g_vSplineOffset;

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
    if (g_uiUseDepthBias)
        Out.vPosition = mul(float4(In.vPosition + In.vNormal * (1.f + g_fDepthBias), 1.f), g_matWVP);
    else
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

VS_OUT VS_SPLINE_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float3 vLocalPos = In.vPosition + g_vSplineOffset.xyz;
    
    //spline
    float t         = saturate(vLocalPos.z / g_vSplineOffset.w);
    float segmentF  = t * (NUMSPLINESAMPLE - 1);
    float localT    = frac(segmentF);
    int idx0        = (int) segmentF;
    int idx1        = min(idx0 + 1, NUMSPLINESAMPLE - 1);
    
    // spline sample
    float3 vUp, vForward, vRight;
    
    float3 p0 = g_vSplinePos[idx0].xyz;
    float3 p1 = g_vSplinePos[idx1].xyz;

    float3 vSplinePos = lerp(p0, p1, localT);
    float3 tan0 = normalize(g_vSplineTanget[idx0].xyz);
    float3 tan1 = normalize(g_vSplineTanget[idx1].xyz);
    vForward = normalize(lerp(tan0, tan1, localT));
    vUp = float3(0, 1, 0);
    if (abs(dot(vUp, vForward)) > 0.99f) vUp = float3(1, 0, 0);
    vRight  = normalize(cross(vUp, vForward));
    vUp = normalize(cross(vForward, vRight));
    
    // local offset
    float3 vWorldPos =
        vSplinePos              +
        vRight  * vLocalPos.x   +
        vUp     * vLocalPos.y;
    
    // tangent space
    float3x3 vBasis = {
        vRight,
        vUp,
        vForward
    };
    
    Out.vPosition   = mul(mul(float4(vWorldPos, 1.f), g_ViewMatrix),g_ProjMatrix);
    Out.vNormal     = normalize(mul(In.vNormal, vBasis));
    Out.vTexcoord0  = In.vTexcoord0;
    Out.vTexcoord1  = In.vTexcoord1;
    Out.vTexcoord2  = In.vTexcoord2;
    Out.vTexcoord3  = In.vTexcoord3;
    Out.vProjPos    = Out.vPosition;
    Out.vTangent    = normalize(mul(In.vTangent,    vBasis));
    Out.vBinormal   = normalize(mul(In.vBinormal,   vBasis));
    Out.vWorld      = vWorldPos;
    
    return Out;
}

VS_OUT VS_OL(VS_IN In)
{
    VS_OUT Out;
    Out.vPosition = mul(float4(In.vPosition + In.vNormal * g_fOL, 1.f), g_matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord0 = In.vTexcoord0;
    Out.vTexcoord1 = In.vTexcoord1;
    Out.vTexcoord2 = In.vTexcoord2;
    Out.vTexcoord3 = In.vTexcoord3;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), g_WorldMatrix)).xyz;
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
    if (g_bUseMC)
    {
        float4 f4Point = g_HBKTextures[TEXTURE_TYPE_BC].Sample(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV));
        if (f4Point.a < 0.1f)
            discard;
        Out.vDiffuseB.rgb = g_MCTexture.Sample(PointSampler, float2(0.5f, f4Point.x)).rgb;
    }
    else if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_BC))
    {
        float4 f4Diffuse = g_HBKTextures[TEXTURE_TYPE_BC].SampleLevel(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV), g_fMipMap);
        if (f4Diffuse.a < 0.1f)
            discard;
        Out.vDiffuseB.rgb = f4Diffuse.rgb + g_vDiffusePlus;
    }
    else
        Out.vDiffuseB = 0.f;
    if (g_uiUseCustomTexture)
    {
        float4 f4Diffuse = g_CustomTexture.Sample(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV));
        if (f4Diffuse.a < 0.1f)
            discard;
        Out.vDiffuseB.rgb = f4Diffuse.rgb + g_vDiffusePlus;
    }
    
    // Shadow
    float3 vShadow;
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_SBC))
    {
        vShadow = g_HBKTextures[TEXTURE_TYPE_SBC].SampleLevel(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV), g_fMipMap).rgb;
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
            g_HBKTextures[TEXTURE_TYPE_N].SampleLevel(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV), g_fMipMap).xyz * 2.f - 1.f,
            float3x3(vTangent, vBinormal * -1.f, vNormal)
        );
    }
    else
        Out.vNormal.xyz = In.vNormal;
    if (Out.vNormal.a > 0.5f && saturate(dot(normalize(g_vLightDir) * -1.f, Out.vNormal.xyz)) < g_fShadowJudge)
        Out.vDiffuseB.rgb = vShadow;
    
    // Emissive
    if (g_uiUseCustomEmissive)
    {
        Out.vEmissive.rgb = g_vEmissiveColor;
        Out.vDiffuseB.a = 1.f;
    }
    else if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_E))
    {
        Out.vEmissive = g_HBKTextures[TEXTURE_TYPE_E].SampleLevel(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV), g_fMipMap).xyz * g_vEmissiveColor;
        Out.vDiffuseB.a = 1.f; // No Blur for them with EmissiveMask
    }
    else
    {
        Out.vEmissive.rgb = g_vEmissiveColor;
        Out.vDiffuseB.a = 0.f;
    }
    if (g_uiUseCustomTexture)
    {
        Out.vEmissive = g_CustomTexture.SampleLevel(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV), g_fMipMap).xyz * g_vEmissiveColor;
        Out.vDiffuseB.a = 1.f;
    }
    float2 f2UV = !g_uiUVNum ? In.vTexcoord0 : (g_uiUVNum == 1 ? In.vTexcoord1 : (g_uiUVNum == 2 ? In.vTexcoord2 : In.vTexcoord3));
    if (g_uiDiscardX)
    {
        if (g_f2DiscardLimit.x <= g_f2DiscardLimit.y)
        {
            if (f2UV.x <= g_f2DiscardLimit.x || g_f2DiscardLimit.y <= f2UV.x)
                Out.vEmissive = 0.f;
        }
        else if (g_f2DiscardLimit.x < f2UV.x || f2UV.x < g_f2DiscardLimit.y)
            Out.vEmissive = 0.f;
    }
    if (g_uiDiscardY)
    {
        if (g_f2DiscardLimit.x <= g_f2DiscardLimit.y)
        {
            if (f2UV.y <= g_f2DiscardLimit.x || g_f2DiscardLimit.y <= f2UV.y)
                Out.vEmissive = 0.f;
        }
        else if (g_f2DiscardLimit.x < f2UV.y || f2UV.y < g_f2DiscardLimit.y)
            Out.vEmissive = 0.f;
    }
    
    // Mask
    bool bMask = false;
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_EL))
    {
        float fEL = g_HBKTextures[TEXTURE_TYPE_EL].Sample(DefaultSampler, In.vTexcoord1).x;
        if (fEL < EPS)
            bMask = true;
        Out.vDiffuseB.rgb = lerp(float3(0.f, 0.f, 0.f), Out.vDiffuseB.rgb, fEL);
    }
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_NC))
    {
        float fNC = g_HBKTextures[TEXTURE_TYPE_NC].Sample(DefaultSampler, In.vTexcoord2).x;
        if (fNC < EPS)
            bMask = true;
        Out.vDiffuseB.rgb = lerp(float3(0.f, 0.f, 0.f), Out.vDiffuseB.rgb, fNC);
    }
    if (g_uiUseCustomDiffuse && !bMask)
    {
        Out.vDiffuseB.rgb = g_vDiffuseColor;
        Out.vNormal.a = 0.1f;
    }
    
    // Roughness
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_MBR) && In.vTexcoord0.y <= 0.5f)
        Out.vELSR.b = g_HBKTextures[TEXTURE_TYPE_MBR].SampleLevel(ClampSampler, In.vTexcoord0 + float2(g_fU, g_fV), g_fMipMap).x;
    else if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_R) && In.vTexcoord0.y <= 0.5f)
        Out.vELSR.b = g_HBKTextures[TEXTURE_TYPE_R].SampleLevel(ClampSampler, In.vTexcoord0 + float2(g_fU, g_fV), g_fMipMap).x;
    else
        Out.vELSR.b = 0.f;
    
    // EdgeLine
    if (g_bUseHBKTextureMask & (1 << TEXTURE_TYPE_EL))
        Out.vELSR.r = g_HBKTextures[TEXTURE_TYPE_EL].SampleLevel(DefaultSampler, In.vTexcoord0 + float2(g_fU, g_fV), g_fMipMap).x;
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
    else Out.vELSR.g = 0.f;
    if (g_uiUseCustomSpecular)
    {
        float2 f2UV = GetWorldSpaceUV(In.vWorld, In.vNormal);
        float fCam = (g_camPosition.x + g_camPosition.y + g_camPosition.z) * length(g_camPosition) / max(abs(g_camPosition.x) + abs(g_camPosition.y) + abs(g_camPosition.z), EPS);
        float fSpecular = g_SpecularTexture.Sample(DefaultSampler, float2(frac((fCam + f2UV.x + f2UV.y) * 0.025f) * 4.f, 0)).r;
        Out.vELSR.g = step(0.5f, fSpecular);
    }
    
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
    if (g_uiUseCustomEmissive)
    {
        Out.vEmissive.rgb = g_vEmissiveColor;
        Out.vDiffuseB.a = 1.f;
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
    if (g_uiUseCustomDiffuse)
        Out.vELSR.r = 0.f;
    
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
    if (g_fPenetration > EPS)
        Out.vGlass.x = -1.f;
    else
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
        if (g_fReverseGlass > EPS)
            Out.vELSR.g = Out.vELSR.g ? 0.f : 1.f;
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

struct PS_OUT_BLEND
{
    float4 vColor : SV_Target0;
};

struct PS_OUT_UNLIT
{
    float4 vColor : SV_Target0;
    float1 vDepth : SV_Target1;
};

float2 g_vBarrierUVScale = { 8.f, 8.f };
PS_OUT_BLEND PS_BARRIER(PS_IN In)
{
    PS_OUT_BLEND Out;
  
    float3 vColor0 = { 0.153f, 0.29f, 0.565f };
    float3 vColor1 = { 0.204f, 0.329f, 0.475f };
    float3 vColor2 = { 0.616f, 0.87f, 1.425f };
    
    float2 vUV = In.vTexcoord0;
    vUV *= g_vBarrierUVScale;
    
    float w0 = g_HBKTextures[TEXTURE_TYPE_BC]. Sample(DefaultSampler, vUV);
    float w2 = g_HBKTextures[TEXTURE_TYPE_SPM].Sample(DefaultSampler, vUV);
    float w1 = 1.f - w0 - w2;
    
    Out.vColor.rgb =
    vColor0 * w0 +
    vColor1 * w1 +
    vColor2 * w2;
    
    Out.vColor.a = 1.f;
    
    
    
    return Out;
}

PS_OUT_UNLIT PS_ZSHIELD(PS_IN In)
{
    PS_OUT_UNLIT Out;
  
    float3 vColor0 = { 2.17f, 0.682f, 0.545f };
    float3 vColor1 = { 1.294f, 0.357f, 0.325f };
    float3 vColor2 = { 0.867f, 0.239f, 0.18f };
    
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
    Out.vDepth = In.vProjPos.z / In.vProjPos.w;
    
    return Out;
}

PS_OUT_UNLIT PS_CORE(PS_IN In)
{
    PS_OUT_UNLIT Out;
    Out.vDepth = In.vProjPos.z / In.vProjPos.w;
    float3 f3BG = { 0.243f, 0.035f, 0.063f };
    float3 f3C1 = { 0.345f, 0.027f, 0.078f };
    float3 f3C2 = { 0.875f, 0.078f, 0.086f };
    float2 f2UV = frac(In.vTexcoord0 + float2(g_fU, g_fV));
    
    uint uiX = (uint) floor(f2UV.x * 112.f) % 4;
    if (!uiX || uiX == 2)
    {
        Out.vColor = float4(f3BG, 1.f);
        return Out;
    }
    uint uiYPlus = 0;
    if (uiX == 1)
        uiYPlus = 2;
    
    uint uiY = ((uint) floor(f2UV.y * 64.f) + uiYPlus) % 4;
    if (!uiY || uiY == 2)
    {
        Out.vColor = float4(f3BG, 1.f);
        return Out;
    }
    if (uiY == 1)
    {
        Out.vColor = float4(f3C1, 1.f);
        return Out;
    }
    Out.vColor = float4(f3C2, 1.f);
    return Out;
}

PS_OUT_UNLIT PS_OL(PS_IN In)
{
    PS_OUT_UNLIT Out;
    Out.vDepth = In.vProjPos.z / In.vProjPos.w;
    Out.vColor = float4(g_vDiffuseColor, 1.f);
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
        SetDepthStencilState(DSS_Depth_NoWrite, StencilEffect);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
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
    pass SplinePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SPLINE_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass CorePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect_DepthWrite, StencilEffect);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CORE();
    }
    pass OLPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Effect_DepthWrite, StencilEffect);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_OL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OL();
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















