#include "Shader_Defines.hlsli"
#include "Shader_Util.hlsli"
#include "Shader_PBR.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
vector g_vCamPosition;

float       g_fBloomIntensity   = 9.f;
float       g_fBloomThreshold   = 0.f;

#define OutDoor 0
#define InDoor 1
uint        g_uiEnvironment = OutDoor;

//Halftone
float       g_fDensityOfDots;
float       g_fHalftoneRadius;

Texture2D   g_SceneTexture;
Texture2D   g_BloomTexture;
Texture2D   g_EmissiveTexture;
Texture2D   g_OLTexture;
Texture2D   g_ELSRTexture;
Texture2D   g_DepthTexture;
Texture2D   g_NormalTexture;
Texture2D   g_BTexture;
Texture2D   g_EBTexture;
    
float3      g_vEdgeColor = { 0.f, 0.f, 0.f };
float3      g_vSpecularColor = { 1.f, 1.f, 1.f };
float       g_fSpecularValue = 1.f;
uint        g_uiShutDown = 0.f;
float       g_fLowHP = 0.f;

Texture2D   g_TargetSampleTexture;
Texture2D   g_DownSampleTexture;
float2      g_vTexelSize;
float       g_fBlurDepthLimit = 0.999f;

Texture2D   g_UpSampleTexture0;
Texture2D   g_UpSampleTexture1;
Texture2D   g_UpSampleTexture2;
Texture2D   g_UpSampleTexture3;

float weights[5] =
{
    0.227027,
    0.1945946,
    0.1216216,
    0.054054,
    0.016216
};

//Additional Blur
float g_fShallowWeights[8];
float g_fDeepWeights[8];

//Color Reverse
float g_fReverse;

//Camera Motion
//float g_fTime = 0.f;
//float g_fSpeedScale = 1.f;
//float g_fCamDistortionStrength = 0.f;
//float3 g_vCamVelo = 0.f;

//Distortion
float2  g_vDistortionPos        = 0.f;
float2   g_vDistortionRange      = 0.f;
float   g_fDistortionStrength   = 0.f;

//Wave
float4 g_vWavePosXY[5];
float4 g_vWaveParamRSD[5];  //Range, Strength, Distance

float3 ACESFilm(float3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;

    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

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

PS_OUT PS_DOWNSAMPLE(PS_IN In)
{
    PS_OUT Out;
    
    float3 color =
        g_TargetSampleTexture.Sample(ClampSampler, In.vTexcoord + float2(-0.5, -0.5) * g_vTexelSize).xyz +
        g_TargetSampleTexture.Sample(ClampSampler, In.vTexcoord + float2(+0.5, -0.5) * g_vTexelSize).xyz +
        g_TargetSampleTexture.Sample(ClampSampler, In.vTexcoord + float2(-0.5, +0.5) * g_vTexelSize).xyz +
        g_TargetSampleTexture.Sample(ClampSampler, In.vTexcoord + float2(+0.5, +0.5) * g_vTexelSize).xyz;

    color *= 0.25;

    Out.vColor = vector(color, 1.f);

    return Out;
}

PS_OUT PS_UPSAMPLE(PS_IN In)
{
    PS_OUT Out;

    float3 color =
        g_UpSampleTexture0.Sample(ClampSampler, In.vTexcoord).xyz * 0.f +
        g_UpSampleTexture1.Sample(ClampSampler, In.vTexcoord).xyz * 1.f +
        g_UpSampleTexture2.Sample(ClampSampler, In.vTexcoord).xyz * 2.f +
        g_UpSampleTexture3.Sample(ClampSampler, In.vTexcoord).xyz * 1.f;

    Out.vColor = vector(color, 1.f);

    return Out;
}

PS_OUT PS_BLOOMEXTRACT(PS_IN In)
{
    PS_OUT Out;
    
    vector _vSceneColor = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    float brightness = max(max(_vSceneColor.r, _vSceneColor.g), _vSceneColor.b);
    float mask = saturate(brightness);

    Out.vColor = float4(min(_vSceneColor.rgb, 1000) * mask * g_fBloomIntensity, 1);

    return Out;
}

PS_OUT PS_FOG(PS_IN In)
{
    PS_OUT Out;

    vector _vSceneColor = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    float brightness = max(max(_vSceneColor.r, _vSceneColor.g), _vSceneColor.b);
    float mask = saturate((brightness - 1.0));

    Out.vColor = float4(_vSceneColor.rgb * mask, 1);

    return Out;
}

PS_OUT PS_BLURH(PS_IN In)
{
    PS_OUT Out;
    float depth = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord).x;
    float3 result = g_TargetSampleTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    float sum = 1.f;
    if (depth < g_fBlurDepthLimit)
    {
        sum = 0.f;
        result *= weights[0];
        sum += weights[0];
        [unroll]
        for (int i = 1; i < 5; i++)
        {
            result += g_TargetSampleTexture.SampleLevel(ClampSampler, In.vTexcoord + float2(g_vTexelSize.x * i, 0), 0).rgb * weights[i];
            result += g_TargetSampleTexture.SampleLevel(ClampSampler, In.vTexcoord - float2(g_vTexelSize.x * i, 0), 0).rgb * weights[i];
            sum += weights[i] * 2.f;
        }
    }
    Out.vColor = float4(result / sum, 1);
    return Out;
}

PS_OUT PS_BLURV(PS_IN In)
{
    PS_OUT Out;
    float depth = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord).x;
    float3 result = g_TargetSampleTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    float sum = 1.f;
    if (depth < g_fBlurDepthLimit)
    {
        sum = 0.f;
        result *= weights[0];
        sum += weights[0];
        [unroll]
        for (int i = 1; i < 5; i++)
        {
            result += g_TargetSampleTexture.SampleLevel(ClampSampler, In.vTexcoord + float2(0, g_vTexelSize.y * i), 0).rgb * weights[i];
            result += g_TargetSampleTexture.SampleLevel(ClampSampler, In.vTexcoord - float2(0, g_vTexelSize.y * i), 0).rgb * weights[i];
            sum += weights[i] * 2.f;
        }
    }
    Out.vColor = float4(result / sum, 1);
    return Out;
}

PS_OUT PS_COMBINED(PS_IN In)
{
    PS_OUT Out;

    float fBlur = g_BTexture.Sample(DefaultSampler, In.vTexcoord).a;
    float fEBlur = g_EBTexture.Sample(DefaultSampler, In.vTexcoord).x;
    float fB = fBlur * fEBlur;
    float fDepth = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord).x;
    vector _vBloomColor = g_BloomTexture.Sample(DefaultSampler, In.vTexcoord);
    float fMaxColor = max(max(_vBloomColor.r, _vBloomColor.g), _vBloomColor.b) - g_fBloomThreshold;
    float3 vColor;
    if (fB < 0.5f && fMaxColor > 0.f)
    {
        float fHalftone;
        if (g_uiEnvironment == OutDoor)
          fHalftone = Halftone(In.vTexcoord, g_fDensityOfDots, g_fHalftoneRadius * fMaxColor);
        else
        {
            float3 f3Normal = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord).xyz;
            if (dot(f3Normal, f3Normal) > 1.1f)
                f3Normal = float3(0.f, 0.f, 1.f);
            float4 f4WorldPos = mul(float4(In.vTexcoord.x * 2.f - 1.f, In.vTexcoord.y * -2.f + 1.f, fDepth, 1.f), g_ProjMatrixInv);
            f4WorldPos /= f4WorldPos.w;
            f4WorldPos = mul(f4WorldPos, g_ViewMatrixInv);
            fHalftone = Halftone(GetWorldSpaceUV(f4WorldPos.xyz, f3Normal), g_fDensityOfDots, g_fHalftoneRadius * fMaxColor);
        }
        vColor = _vBloomColor.rgb * fHalftone;// * fMaxColor;
    }
    else
    {
        _vBloomColor = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);
        vColor = _vBloomColor.rgb * g_fBloomIntensity;
    }
         
    Out.vColor = float4(vColor, 1.f);
    
    return Out;
}

PS_OUT PS_OUTLINE(PS_IN In)
{
    PS_OUT Out;

    vector _vSceneColor = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);

    float3 vColor = _vSceneColor.rgb;
    vColor = lerp(vColor, g_vEdgeColor,     g_OLTexture.Sample(DefaultSampler, In.vTexcoord).r);
    vColor = lerp(vColor, g_vSpecularColor, g_ELSRTexture.Sample(DefaultSampler, In.vTexcoord).g * g_fSpecularValue * (1.f - (float) g_uiShutDown));
    
    Out.vColor = float4(vColor, 1);
    
    return Out;
}

PS_OUT PS_Copy(PS_IN In)
{
    PS_OUT Out;
    Out.vColor = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    return Out;
}

PS_OUT PS_FINAL(PS_IN In)
{
    PS_OUT Out;
    float2 f2UV = In.vTexcoord;
    
    //Camera Motion
    //float2 f2Center = f2UV - 0.5f;
    //float fEdgeMask = smoothstep(0.25f, 1.f, length(f2Center) * 2.f);
    //
    //float fLateralSpeed = saturate(length(g_vCamVelo.xy) * g_fSpeedScale);
    //float2 f2LateralDir = normalize(g_vCamVelo.xy + EPS);
    //float fLateralBand = frac(dot(f2UV, float2(-f2LateralDir.y, f2LateralDir.x)) * 60.f + Noise21(f2UV * 12.f + g_fTime * 0.3f) * 2.5f + Noise21(f2UV * 28.f - g_fTime * 0.5f) * 0.8f + g_fTime * 4.f);
    //float fLateralWave = smoothstep(0.15f, 0.5f, fLateralBand) * (1.f - smoothstep(0.5f, 0.85f, fLateralBand)) * 2.f - 1.f;
    //float2 f2LateralOffset = f2LateralDir * fLateralWave * fLateralSpeed;
    //f2UV += f2LateralOffset * g_fCamDistortionStrength * fEdgeMask / float2(1280.f, 720.f);
    
    //Distortion
    if (g_fDistortionStrength > EPS && g_vDistortionRange.x > EPS && g_vDistortionRange.y > EPS)
    {
        float2 f2Local = (f2UV - g_vDistortionPos) / g_vDistortionRange;
        float fDot = dot(f2Local, f2Local);
        if (fDot < 1.f)
        {
            float fR = sqrt(fDot);
            float2 f2Dir = (fR > EPS) ? (f2Local / fR) : float2(0.f, 0.f);
            f2UV -= f2Dir * fR * (1.f - fR) * g_fDistortionStrength;
        }
    }
    
    //Wave
    float2 f2WavePower = 0.f;
    for (int i = 0; i < 5; ++i)
        if (g_vWaveParamRSD[i].y > EPS && g_vWaveParamRSD[i].x > EPS)
        {
            float2 f2Fix = float2(16.f / 9.f, 1.f);
            float2 f2Local = (In.vTexcoord - g_vWavePosXY[i].xy) * f2Fix;
            float fDist = length(f2Local);
            float fDiff = fDist - g_vWaveParamRSD[i].z;
            float fAbs = abs(fDiff);
            if (fAbs < g_vWaveParamRSD[i].x)
            {
                float2 f2Dir = (fAbs > EPS && fDist > EPS) ? (f2Local / fDist) * (fDiff / fAbs) : float2(0.f, 0.f);
                float fPower = saturate(1.f - fAbs / g_vWaveParamRSD[i].x);
                f2WavePower += f2Dir * fPower * fPower * g_vWaveParamRSD[i].y;
            }
        }
    f2UV += f2WavePower;
    
    float fReverse = saturate(g_fReverse);
    vector _vSceneColor = g_SceneTexture.Sample(DefaultSampler, f2UV);
    Out.vColor = vector(_vSceneColor.rgb + fReverse - 2.f * _vSceneColor.rgb * fReverse, 1.f);
    Out.vColor.r = lerp(Out.vColor.r, 1.f, saturate(g_fLowHP * (max(abs(In.vTexcoord.x - 0.5f), abs(In.vTexcoord.y - 0.5f)) - 0.3f)));
    return Out;
}

PS_OUT PS_BLURX(PS_IN In)
{
    PS_OUT Out;
    float3 f3Color = 0.f;
    float fSum = 0.f;
    [loop]
    for (int i = -7; i < 8; ++i)
    {
        float fX = In.vTexcoord.x + i / WINSIZEX;
        float fWeight;
        if (g_DepthTexture.Sample(DefaultSampler, In.vTexcoord).x < 0.01f)
            fWeight = g_fShallowWeights[abs(i)];
        else
            fWeight = g_fDeepWeights[abs(i)];
        f3Color += fWeight * g_SceneTexture.Sample(ClampSampler, float2(fX, In.vTexcoord.y));
        if (fX >= 0.f && fX <= 1.f)
            fSum += fWeight;
    }
    Out.vColor = float4(f3Color / fSum, 1.f);
    return Out;
}

PS_OUT PS_BLURY(PS_IN In)
{
    PS_OUT Out;
    float3 f3Color = 0.f;
    float fSum = 0.f;
    [loop]
    for (int i = -7; i < 8; ++i)
    {
        float fY = In.vTexcoord.y + i / WINSIZEY;
        float fWeight;
        if (g_DepthTexture.Sample(DefaultSampler, In.vTexcoord).x < 0.01f)
            fWeight = g_fShallowWeights[abs(i)];
        else
            fWeight = g_fDeepWeights[abs(i)];
        f3Color += fWeight * g_SceneTexture.Sample(ClampSampler, float2(In.vTexcoord.x, fY));
        if (fY >= 0.f && fY <= 1.f)
            fSum += fWeight;
    }
    Out.vColor = float4(f3Color / fSum, 1.f);
    return Out;
}

technique11 DefaultTechnique
{
    pass DOWNSAMPLE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DOWNSAMPLE();
    }
    pass UPSAMPLE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UPSAMPLE();
    }
    pass BLOOMEXTRACT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOMEXTRACT();
    }
    pass BLURH
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLURH();
    }
    pass BLURV
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLURV();
    }
    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COMBINED();
    }
    pass Copy
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Copy();
    }
    pass Outline
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth_Masking, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OUTLINE();
    }
    pass Final
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FINAL();
    }
    pass BlurX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLURX();
    }
    pass BlurY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None_Depth, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLURY();
    }
}