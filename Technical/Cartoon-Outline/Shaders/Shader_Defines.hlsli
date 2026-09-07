#ifndef SHADER_DEFINE_HLSLI
#define SHADER_DEFINE_HLSLI

#define WINSIZEX 1920.0
#define WINSIZEY 1080.0

#define PI 3.1415926535


sampler DefaultSampler = sampler_state
{
    Filter = Min_Mag_Mip_Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler ClampSampler = sampler_state
{
    Filter = Min_Mag_Mip_Linear;
    AddressU = Clamp;
    AddressV = Clamp;
};

sampler PointSampler = sampler_state
{
    Filter = Min_Mag_Mip_Point;
    AddressU = Clamp;
    AddressV = Clamp;
};

/* RasterizerState*/
RasterizerState RS_Default
{
    FillMode = solid;
    CullMode = Back;
};

RasterizerState RS_Cull_None
{
    FillMode = solid;
    CullMode = None;
};

RasterizerState RS_Wireframe
{
    FillMode = wireframe;
};

RasterizerState RS_Shadow
{
    FillMode = solid;
    CullMode = None;
    DepthClipEnable = true;
};

RasterizerState RS_Front
{
    FillMode = solid;
    CullMode = Front;
};

RasterizerState RS_Mask
{
    FillMode = solid;
    CullMode = Back;

    DepthBias = -20;

    DepthClipEnable = true;
};

/* DepthStencilState*/
#define StencilCharacter 1
#define StencilEffect 2
DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_Depth_NoWrite
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less_equal;

    StencilEnable = true;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;
    FrontFaceStencilPass = replace;
    BackFaceStencilPass = replace;
};

DepthStencilState DSS_Effect_DepthWrite
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;

    StencilEnable = true;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;
    FrontFaceStencilPass = replace;
    BackFaceStencilPass = replace;
};

DepthStencilState DSS_Character // RefValue : 1
{
    DepthEnable     = true;
    DepthWriteMask  = all;
    DepthFunc       = less_equal;

    StencilEnable   = true;
    StencilReadMask = 0xFF;
    StencilWriteMask= 0xFF;

    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;
    FrontFaceStencilPass = replace;
    BackFaceStencilPass = replace;
};

DepthStencilState DSS_None_Depth
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_None_Depth_Masking
{
    DepthEnable     = false;
    DepthWriteMask = zero;

    StencilEnable   = true;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFunc = equal;
    FrontFaceStencilPass = keep;
    BackFaceStencilFunc = equal;
    BackFaceStencilPass = keep;
};

DepthStencilState DSS_None_Depth_StencilNotEqual
{
    DepthEnable = false;
    DepthWriteMask = zero;

    StencilEnable = true;
    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFunc = not_equal;
    FrontFaceStencilPass = keep;
    FrontFaceStencilFail = keep;
    FrontFaceStencilDepthFail = keep;

    BackFaceStencilFunc = not_equal;
    BackFaceStencilPass = keep;
    BackFaceStencilFail = keep;
    BackFaceStencilDepthFail = keep;
};

/* Blend State*/

BlendState BS_Default
{
    BlendEnable[0] = false;
    BlendEnable[1] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

BlendState BS_PreMultiply
{
    BlendEnable[0] = true;
    SrcBlend[0] = One;
    DestBlend[0] = INV_SRC_ALPHA;
    BlendOp[0] = Add;
    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = INV_SRC_ALPHA;
    BlendOpAlpha[0] = Add;

    BlendEnable[1] = true;
    SrcBlend[1] = One;
    DestBlend[1] = One;
    BlendOp[1] = Min;
    SrcBlendAlpha[1] = One;
    DestBlendAlpha[1] = One;
    BlendOpAlpha[1] = Min;
};

BlendState BS_Max
{
    BlendEnable[0] = true;
    SrcBlend = one;
    DestBlend = one;
    BlendOp = max;
    SrcBlendAlpha = one;
    DestBlendAlpha = one;
    BlendOpAlpha = max;
};

BlendState BS_WeightedBlend
{
    AlphaToCoverageEnable = false;
    //IndependentBlendEnable = true;

    BlendEnable[0] = true;
    SrcBlend[0] = one;
    DestBlend[0] = one;
    BlendOp[0] = add;
    SrcBlendAlpha[0] = one;
    DestBlendAlpha[0] = one;
    BlendOpAlpha[0] = add;
    RenderTargetWriteMask[0] = 0x0f;

    BlendEnable[1] = true;
    SrcBlend[1] = zero;
    DestBlend[1] = inv_src_color;
    BlendOp[1] = add;
    SrcBlendAlpha[1] = zero;
    DestBlendAlpha[1] = inv_src_alpha;
    BlendOpAlpha[1] = add;
    RenderTargetWriteMask[1] = 0x0f;

    BlendEnable[2] = true;
    SrcBlend[2] = one;
    DestBlend[2] = one;
    BlendOp[2] = min;
    SrcBlendAlpha[2] = one;
    DestBlendAlpha[2] = one;
    BlendOpAlpha[2] = min;
    RenderTargetWriteMask[2] = 0x0f;
};

BlendState BS_AlphaBlend_Color_AlphaMax
{
    BlendEnable[0] = true;

    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;

    SrcBlendAlpha = one;
    DestBlendAlpha = one;
    BlendOpAlpha = max;

    RenderTargetWriteMask[0] = 0x0f;
};

BlendState BS_ColorMax
{
    BlendEnable[0] = true;
    SrcBlend = one;
    DestBlend = one;
    BlendOp = max;
};

BlendState BS_GodRay
{
    BlendEnable[0] = true;

    SrcBlend[0] = one;
    DestBlend[0] = inv_src_color;
    BlendOp[0] = add;

    SrcBlendAlpha[0] = zero;
    DestBlendAlpha[0] = one;
    BlendOpAlpha[0] = add;

    RenderTargetWriteMask[0] = 0x0f;
};
#endif
