
#include "Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4      g_camPosition;


texture2D g_SplatMapTexture;

texture2D g_SplatTexture0[3];
texture2D g_SplatTexture1[3];
texture2D g_SplatTexture2[3];
texture2D g_SplatTexture3[3];
float4    g_vSplatScale = { 1.f, 1.f, 1.f , 1.f };

struct VS_IN
{
    float3 vPosition    : POSITION;
    float3 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition    : SV_POSITION;
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    float4 vWorldPos    : TEXCOORD0;
    float2 vTexcoord    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float4x4 matWV, matWVP;

    matWV   = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP  = mul(matWV, g_ProjMatrix);

    Out.vPosition   = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord   = In.vTexcoord;
    Out.vWorldPos   = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos    = Out.vPosition;

    float3 WorldN   = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix).rgb);
    float3 ref      = float3(1, 0, 0);
    float3 T        = normalize(ref - WorldN * dot(ref, WorldN));
    float3 B        = cross(WorldN, T);

    Out.vNormal     = WorldN;
    Out.vBinormal   = B;
    Out.vTangent    = T;
    
    return Out;
}


struct PS_IN
{
    float4 vPosition    : SV_POSITION;
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
    float4 vWorldPos    : TEXCOORD0;
    float2 vTexcoord    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_DEFFERED_OUT
{
    float4 vDiffuse     : SV_TARGET0;
    float4 vMRAOS       : SV_TARGET1;
    float4 vEmissive    : SV_TARGET2;
    float4 vNormal      : SV_TARGET3;
    float4 vDepth       : SV_TARGET4;
};

PS_OUT PS_WIREFRAME_MAIN(PS_IN In)
{
    PS_OUT Out;

    vector vColor = { 0.0f, 1.0f, 0.0f, 1.0f };
    Out.vColor = vColor;

    return Out;
};

PS_OUT PS_SPLATMAP_MAIN(PS_IN In)
{
    PS_OUT Out;

    Out.vColor = g_SplatMapTexture.Sample(DefaultSampler, In.vTexcoord);

    return Out;
};

PS_DEFFERED_OUT PS_MAIN(PS_IN In)
{
    PS_DEFFERED_OUT Out;

    vector vMtrlDiffuse     = { 0.f, 0.f, 0.f, 1.f };
    vector vMtrlMRAOS       = { 0.f, .5f, 1.f, 0.f };
    vector vMtrlEmissive    = { 0.f, 0.f, 0.f, 1.f };
    vector vMtrlNormal      = vector(In.vNormal.xyz, 0.f);

    //¹º°¡ splatmapÀÌ¶û ÅØ½ºÃÄ·Î °³Â¼´Â ¹«¾ð°¡
    vector vSplatColor0[3], vSplatColor1[3], vSplatColor2[3], vSplatColor3[3];
    for (int i = 0; i < 3; i++) vSplatColor0[i] = g_SplatTexture0[i].Sample(DefaultSampler, In.vTexcoord * g_vSplatScale.x);
    for (int i = 0; i < 3; i++) vSplatColor1[i] = g_SplatTexture1[i].Sample(DefaultSampler, In.vTexcoord * g_vSplatScale.y);
    for (int i = 0; i < 3; i++) vSplatColor2[i] = g_SplatTexture2[i].Sample(DefaultSampler, In.vTexcoord * g_vSplatScale.z);
    for (int i = 0; i < 3; i++) vSplatColor3[i] = g_SplatTexture3[i].Sample(DefaultSampler, In.vTexcoord * g_vSplatScale.w);

    vector vSplatWeight     = g_SplatMapTexture.Sample(DefaultSampler,  In.vTexcoord);
    vector vSplatHeight     = { vSplatColor0[2].x, vSplatColor1[2].x, vSplatColor2[2].x, vSplatColor3[2].x };
    vector vAdjustWeight    = vSplatHeight * vSplatWeight;
    float sum = vAdjustWeight.x + vAdjustWeight.y + vAdjustWeight.z + vAdjustWeight.w;
    vAdjustWeight /= max(sum, 0.0001);

    vector vDiffuseColor    = vAdjustWeight.x * vSplatColor0[0] + vAdjustWeight.y * vSplatColor1[0] + vAdjustWeight.z * vSplatColor2[0] + vAdjustWeight.w * vSplatColor3[0];

    float3 n0 = vSplatColor0[1].xyz * 2 - 1;
    float3 n1 = vSplatColor1[1].xyz * 2 - 1;
    float3 n2 = vSplatColor2[1].xyz * 2 - 1;
    float3 n3 = vSplatColor3[1].xyz * 2 - 1;
    float3 normalTS = vAdjustWeight.x * n0 + vAdjustWeight.y * n1 + vAdjustWeight.z * n2 + vAdjustWeight.w * n3;
    normalTS.z = sqrt(saturate(1.0f - dot(normalTS.xy, normalTS.xy)));

    normalTS = normalize(normalTS);
    float3 vNormal =
        normalTS.x * In.vTangent +
        normalTS.y * In.vBinormal +
        normalTS.z * In.vNormal;
    vNormal = normalize(vNormal);

    float AO        = vAdjustWeight.x * vSplatColor0[2].z + vAdjustWeight.y * vSplatColor1[2].z + vAdjustWeight.z * vSplatColor2[2].z + vAdjustWeight.w * vSplatColor3[2].z;
    float roughness = vAdjustWeight.x * vSplatColor0[2].y + vAdjustWeight.y * vSplatColor1[2].y + vAdjustWeight.z * vSplatColor2[2].y + vAdjustWeight.w * vSplatColor3[2].y;
    
    vMtrlDiffuse.rgb    = vDiffuseColor.rgb;
    vMtrlNormal.xyz     = vNormal.xyz;
    vMtrlMRAOS.g        = roughness;
    vMtrlMRAOS.b        = AO;

    Out.vDiffuse    = vMtrlDiffuse;
    Out.vMRAOS      = vMtrlMRAOS;
    Out.vEmissive   = vMtrlEmissive;
    Out.vNormal     = vector(vMtrlNormal.xyz * 0.5f + 0.5f, vMtrlNormal.w);
    Out.vDepth      = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 5000.f, 0.f, 0.f);


    return Out;
};

technique11 DefaultTechnique
{
    pass WireframePass {
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader     = compile ps_5_0 PS_WIREFRAME_MAIN();
    }
    pass SplatPass {
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader     = compile ps_5_0 PS_SPLATMAP_MAIN();
    }
    pass ShadePass {
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }
}















