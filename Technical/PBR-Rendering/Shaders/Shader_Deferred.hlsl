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

float       g_fIBLIntensity;
texture2D   g_IBLTexture;
bool        g_bPrecomputedRadiance;
bool        g_bUseSplitSum;
bool        g_bPrecomputedSplitSum;
texture2D   g_IrradianceTexture;
texture2D   g_BRDFLUTTexture;
texture2D   g_FilteredEnvTexture;

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

    float3 vSpecularBRDF    = NDF*FF*GSF / max(4 * NdotL * NdotV, 0.001);

    if (vNormalDesc.w > 0.5f) NdotL += saturate(dot(normalize(_vLightDir) * -1.f, -vNormal)) * 0.5f; //나뭇잎ㄲ같이 뒤가 비치는 매질 처리

    float3 L0_Diffuse   = KD * vDiffuseBRDF * g_vLightDiffuse.xyz * NdotL;
    float3 L0_Specular  = vSpecularBRDF     * g_vLightSpecular.xyz * NdotL;
    
    //SSS
    float wrap          = 0.4;
    float rim           = pow(1 - NdotV, 1.712f);
    float wrapDiffuse   = saturate((NdotL + wrap) / (1 + wrap));
    float sssTerm       = (wrapDiffuse * 0.6 + back * 0.4) * rim;
    float3 SSSWeight    = { 1.f, .5f, .25f };
    float3 L0_SSS       = g_vLightDiffuse.xyz * BaseColor * sssTerm * SSS * 0.5 * SSSWeight;
    
    L0_Diffuse  *= g_fLightIntensity;
    L0_Specular *= g_fLightIntensity;
    L0_SSS      *= g_fLightIntensity;
    L0_Specular = min(L0_Specular, 50.f);

    float3 Lo = L0_Diffuse + L0_Specular + L0_SSS;

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

    vector vNormalDesc  = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc   = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMRAOSDesc   = g_MRAOSTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse     = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    vMRAOSDesc.g        = max(vMRAOSDesc.g, 0.04f);

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
    
    float NDF = NDF_GGX_DISNEY(roughness, NdotH);
    float FF = KS;
    float GSF = GSF_Smith(NdotL, NdotV, roughness);
    
    float3 vSpecularBRDF =
        NDF * FF * GSF
        / max(4 * NdotL * NdotV, 0.001);


    if (vNormalDesc.w > 0.5f)
        NdotL += saturate(dot(normalize(_vLightDir) * -1.f, -vNormal)) * 0.5f;
    float _fDist = length(vWorldPos.xyz - _vLightPos.xyz);
    if (_fDist > g_fLightRange)
        discard;
    
    float distSq = max(_fDist * _fDist, 0.01f);
    float inverseSquare = 1.0f / (distSq + 1.f);

    float normalizedDist = _fDist / g_fLightRange;
    float window = saturate(1.0f - pow(normalizedDist, 4.0f));
    window *= window;
    
    float attenuation = inverseSquare * window;
    
    float3 L0_Diffuse = g_vLightDiffuse.xyz * vDiffuseBRDF * NdotL * KD;
    float3 L0_Specular = g_vLightSpecular.xyz * vSpecularBRDF * NdotL;

    float wrap = 0.4;
    float rim = 1 - NdotV;
    float wrapDiffuse = saturate((NdotL + wrap) / (1 + wrap));
    float sssTerm = wrapDiffuse * 0.6 + back * 0.4 * rim;
    float3 L0_SSS = g_vLightDiffuse.xyz * BaseColor * sssTerm * SSS * 0.1;
    
    L0_Diffuse *= g_fLightIntensity * attenuation * 100;
    L0_Specular *= g_fLightIntensity * attenuation * 100;
    L0_SSS *= g_fLightIntensity * attenuation * 100;
    float3 Lo = L0_Diffuse + L0_Specular + L0_SSS;

    Out.vShade = vector(Lo.xyz, 1.f);
    //Out.vDiffuse    = vector(L0_Diffuse.xyz,    1.f);
    //Out.vSpecular   = vector(L0_Specular.xyz,   1.f);
    //Out.vAmbient    = vector(L0_SSS.xyz,    1.f);

    return Out;
}


PS_OUT_LIGHT PS_IBL(PS_IN In)
{
    PS_OUT_LIGHT Out;

    vector vNormalDesc  = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDepthDesc   = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vMRAOSDesc   = g_MRAOSTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vDiffuse     = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);

    /* 0 ~ 1 => -1 ~ 1  */
    float4 vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vNormal.xyz = normalize(vNormal.xyz);
    vector vWorldPos = Depth2World(vDepthDesc, 5000.f, In.vTexcoord, g_ViewMatrixInv, g_ProjMatrixInv);
    vector vLook = vWorldPos - g_vCamPosition;
    vector V = vector(-normalize(vLook.xyz), 0.f);
    
    float3 BaseColor    = vDiffuse.xyz;
    float metalness     = vMRAOSDesc.r;
    float rougness      = vMRAOSDesc.g;
    float AO            = vMRAOSDesc.b;
    float3 KD = (1.f - metalness);
    float3 F0 = SchlickFresnel(BaseColor, metalness);
    rougness = clamp(rougness, 0.04f, 1.0f);
    
    //Diffuse IBL
    float3 irradiance;
    if (g_bPrecomputedRadiance) 
    {
        irradiance = g_IrradianceTexture.Sample(DefaultSampler, DirectionToEquirectUV(vNormal.xyz)).rgb;
    }
    else
    {
        irradiance = Diffuse_IBL_Sample(vNormal.xyz, g_IBLTexture);
    }
    float3 diffuseIBL = KD * BaseColor / PI * irradiance * AO;
    
    //Specular IBL
    float3 specularIBL = float3(0.f, 0.f, 0.f); 
    if (g_bUseSplitSum)
    {
        float NoV = saturate(dot(vNormal, V));
        float3 R = 2 * dot(V, vNormal) * vNormal - V;
        float3 PrefilteredColor;
        float2 EnvBRDF;
        if (g_bPrecomputedSplitSum)
        {
            float2 uvLUT    = float2(rougness, NoV);
            PrefilteredColor = RRoughnessToValue(float4(R, rougness), g_FilteredEnvTexture);
            EnvBRDF          = g_BRDFLUTTexture.Sample(DefaultSampler, uvLUT).xy;
        }
        else
        {
            PrefilteredColor = PrefilterEnvMap(rougness, R, g_IBLTexture);
            EnvBRDF          = IntegrateBRDF(rougness, NoV);
        }
        specularIBL = PrefilteredColor * (F0 * EnvBRDF.x + EnvBRDF.y);

    }
    else
    {
        specularIBL = SpecularIBL(F0, rougness, vNormal.xyz, V.xyz, g_IBLTexture);
    }
    //
    
    Out.vShade = vector((diffuseIBL + specularIBL).xyz * g_fIBLIntensity, 1.f);
    //Out.vShade      = vector(Lo.xyz,            1.f);
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
    vector vDepthDesc       = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos = Depth2World(vDepthDesc, 5000.f, In.vTexcoord, g_ViewMatrixInv, g_ProjMatrixInv);
    vector vLook = vWorldPos - g_vCamPosition;
    
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

    pass Light_IBL
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_IBL();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN_COMBINED();
    }
}