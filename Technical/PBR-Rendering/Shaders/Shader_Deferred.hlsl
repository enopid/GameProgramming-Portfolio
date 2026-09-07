#include "Shader_Defines.hlsli"
#include "Shader_PBR.hlsli"
#include "Shader_Util.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

texture2D   g_Texture;

matrix      g_vLightTransform;
float       g_fLightRange;
float       g_fLightIntensity;
vector      g_vLightAttenuation;

vector      g_vLightDiffuse;
vector      g_vLightAmbient;
vector      g_vLightSpecular;


texture2D   g_DiffuseTexture;
vector      g_vMtrlAmbient = 1.f;
vector      g_vMtrlSpecular = 1.f;
texture2D   g_NormalTexture;
texture2D   g_MRAOSTexture;
texture2D   g_EmissiveTexture;
float       g_fEmissiveIntensity;

texture2D g_ShadeTexture;
texture2D g_SpecularTexture;
texture2D g_DepthTexture;

vector g_vCamPosition;

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

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade   : SV_TARGET0;
    //float4 vDiffuse : SV_TARGET1;
    //float4 vSpecular: SV_TARGET2;
    //float4 vAmbient : SV_TARGET3;
};


PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    vector _vLightDir       = vector(normalize(g_vLightTransform[2].xyz), 0.f);
    vector _vLightScale     = vector(
        length(g_vLightTransform[0].xyz) ,
        length(g_vLightTransform[1].xyz) ,
        length(g_vLightTransform[2].xyz) ,
        0.f 
    );
    vector _vLightPos       = vector(g_vLightTransform[3].xyz, 1.f);

    PS_OUT_LIGHT Out;
    
    vector  vNormalDesc     = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vDepthDesc      = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vMRAOSDesc      = g_MRAOSTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vDiffuse        = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vMRAOSDesc.g = max(vMRAOSDesc.g, 0.04f);
    //vMRAOSDesc.r = max(vMRAOSDesc.r, 0.2f);
    
    /* 0 ~ 1 => -1 ~ 1  */
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);    
    vNormal.xyz = normalize(vNormal.xyz);

    vector vWorldPos = Depth2World(vDepthDesc, 5000.f, In.vTexcoord, g_ViewMatrixInv, g_ProjMatrixInv);
    
    vector vLook = vWorldPos - g_vCamPosition;
    
    vector N = vNormal;
    vector V = vector(-normalize(vLook.xyz),        0.f);
    vector L = vector(-normalize(_vLightDir.xyz),  0.f);
    vector H = normalize(V + L);
    vector R = reflect(L, N);

    float back      = saturate(max(dot(N,-L), 0.01f));
    float NdotL     = saturate(max(dot(N, L), 0.01f));
    float NdotH     = saturate(max(dot(N, H), 0.01f));
    float NdotV     = saturate(max(dot(N, V), 0.01f));
    float VdotH     = saturate(max(dot(V, H), 0.01f));
    float LdotH     = saturate(max(dot(L, H), 0.01f));
    float LdotV     = saturate(max(dot(L, V), 0.01f));
    float RdotV     = saturate(max(dot(R, V), 0.01f));
    float metalness = vMRAOSDesc.r;
    float roughness = vMRAOSDesc.g;
    float AO        = vMRAOSDesc.b;
    float SSS       = vMRAOSDesc.a;

    float3 BaseColor= vDiffuse.xyz;
    float3 F0 = SchlickFresnel(BaseColor, metalness);

    float3 KS = FF_Schlick(F0, VdotH);
    float3 KD = (1.f - KS) * (1.f - metalness);

    float3 vDiffuseBRDF     = BaseColor / PI;

    float NDF = NDF_GGX_DISNEY(roughness, NdotH);
    float FF = KS;
    float GSF = GSF_Smith(NdotL, NdotV, roughness);

    float3 vSpecularBRDF    =
        NDF*FF*GSF
        / max(4 * NdotL * NdotV, 0.001);


    if (vNormalDesc.w > 0.5f) NdotL += saturate(dot(normalize(_vLightDir) * -1.f, -vNormal)) * 0.5f;

    float3 L0_Diffuse   = KD * vDiffuseBRDF * g_vLightDiffuse.xyz * NdotL;
    float3 L0_Specular  = vSpecularBRDF     * g_vLightSpecular.xyz * NdotL;
    float3 L0_Ambient   = g_vLightDiffuse.xyz * g_vLightAmbient.xyz * BaseColor * AO;
    
    float wrap = 0.4;
    float rim = pow(1 - NdotV, 1.712f);
    float wrapDiffuse = saturate((NdotL + wrap) / (1 + wrap));
    float sssTerm = (wrapDiffuse * 0.6 + back * 0.4) * rim;
    float3 SSSWeight = { 1.f, .5f, .25f };
    float3 L0_SSS = g_vLightDiffuse.xyz * BaseColor * sssTerm * SSS * 0.5 * SSSWeight;
    
    L0_Diffuse  *= g_fLightIntensity;
    L0_Specular *= g_fLightIntensity;
    L0_Ambient  *= g_fLightIntensity;
    L0_SSS      *= g_fLightIntensity;
    L0_Specular = min(L0_Specular, 50.f);

    float3 Lo = L0_Diffuse + L0_Specular + L0_Ambient + L0_SSS;

    Out.vShade      = vector(Lo.xyz, 1.f);
    //Out.vDiffuse    = vector(L0_Diffuse.xyz, 1.f);
    //Out.vSpecular   = vector(L0_Specular.xyz, 1.f);
    //Out.vAmbient    = vector(L0_SSS.xyz, 1.f);
    
    return Out;
}


PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    vector _vLightDir = vector(normalize(g_vLightTransform[2].xyz), 0.f);
    vector _vLightScale = vector(
        length(g_vLightTransform[0].xyz),
        length(g_vLightTransform[1].xyz),
        length(g_vLightTransform[2].xyz),
        0.f
    );
    vector _vLightPos = vector(g_vLightTransform[3].xyz, 1.f);

    PS_OUT_LIGHT Out;

    vector  vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vMRAOSDesc = g_MRAOSTexture.Sample(DefaultSampler, In.vTexcoord);
    vector  vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    vMRAOSDesc.g = max(vMRAOSDesc.g, 0.04f);

    /* 0 ~ 1 => -1 ~ 1  */
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vector vWorldPos = Depth2World(vDepthDesc, 5000.f, In.vTexcoord, g_ViewMatrixInv, g_ProjMatrixInv);

    _vLightDir.xyz = normalize(vWorldPos.xyz - _vLightPos.xyz);

    vector vReflect = reflect(normalize(_vLightDir), vNormal);
    vector vLook = vWorldPos - g_vCamPosition;

    vector N = vNormal;
    vector V = vector(-normalize(vLook.xyz), 0.f);
    vector L = vector(-normalize(_vLightDir.xyz), 0.f);
    vector H = normalize(V + L);
    vector R = reflect(L, N);

    float back = saturate(dot(-L, N));
    float NdotL = saturate(max(dot(N, L), 0.01f));
    float NdotH = saturate(max(dot(N, H), 0.01f));
    float NdotV = saturate(max(dot(N, V), 0.01f));
    float VdotH = saturate(max(dot(V, H), 0.01f));
    float LdotH = saturate(max(dot(L, H), 0.01f));
    float LdotV = saturate(max(dot(L, V), 0.01f));
    float RdotV = saturate(max(dot(R, V), 0.01f));
    float metalness = vMRAOSDesc.r;
    float roughness = vMRAOSDesc.g;
    float AO = vMRAOSDesc.b;
    float SSS = vMRAOSDesc.a;

    float3 BaseColor = vDiffuse.xyz;
    float3 F0 = SchlickFresnel(BaseColor, metalness);

    float3 KS = FF_Schlick(F0, VdotH);
    float3 KD = (1.f - KS) * (1.f - metalness);

    float3 vDiffuseBRDF = BaseColor / PI;
    float3 vSpecularBRDF =
        NDF_GGX_DISNEY(roughness, NdotH) *
        F0 *
        GSF_Smith(NdotL, NdotV, roughness)
        / max(4 * NdotL * NdotV, 0.001);


    if (vNormalDesc.w > 0.5f) NdotL += saturate(dot(normalize(_vLightDir) * -1.f, -vNormal)) * 0.5f;
    float _fDist = length(vWorldPos.xyz - _vLightPos.xyz);
    if (_fDist > g_fLightRange) discard;
    float attenuation = 1.0 / _fDist * _fDist;
    float fade = saturate(1.0 - _fDist / g_fLightRange);
    attenuation *= fade * fade;
    
    float3 L0_Diffuse   = g_vLightDiffuse.xyz   * vDiffuseBRDF  * NdotL * KD;
    float3 L0_Specular  = g_vLightSpecular.xyz  * vSpecularBRDF * NdotL;
    float3 L0_Ambient   = g_vLightDiffuse.xyz   * g_vLightAmbient.xyz * BaseColor * AO;

    float wrap = 0.4;
    float rim = 1 - NdotV;
    float wrapDiffuse = saturate((NdotL + wrap) / (1 + wrap));
    float sssTerm = wrapDiffuse * 0.6 + back * 0.4 * rim;
    float3 L0_SSS = g_vLightDiffuse.xyz * BaseColor * sssTerm * SSS * 0.1;
    
    L0_Diffuse  *= g_fLightIntensity * attenuation;
    L0_Specular *= g_fLightIntensity * attenuation;
    L0_Ambient  *= g_fLightIntensity * attenuation;
    L0_SSS      *= g_fLightIntensity * attenuation;
    float3 Lo = L0_Diffuse + L0_Specular + L0_Ambient + L0_SSS;

    Out.vShade      = vector(Lo.xyz,            1.f);
    //Out.vDiffuse    = vector(L0_Diffuse.xyz,    1.f);
    //Out.vSpecular   = vector(L0_Specular.xyz,   1.f);
    //Out.vAmbient    = vector(L0_SSS.xyz,    1.f);

    return Out;
}

PS_OUT PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT Out;
    
    vector vDiffuse         = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vEMMISIIVEDesc   = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vShade           = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);

    if (0.f == vDiffuse.a)
        discard;
    
    Out.vColor = vShade + vector(vEMMISIIVEDesc.xyz, 1.f) * g_fEmissiveIntensity;

    return Out;
}

PS_OUT PS_MAIN_PHOTO(PS_IN In)
{
    PS_OUT Out;
    
    vector vDiffuse         = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    if (0.f == vDiffuse.a) discard;
    
    Out.vColor = vDiffuse;

    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }
}