#ifndef SHADER_DEFINE_HLSLI
#define SHADER_DEFINE_HLSLI


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

/* RasterizerState*/
RasterizerState RS_Default
{
    FillMode = solid;
    CullMode = Back;
};

RasterizerState RS_Cull_Front
{
    CullMode = front;
};

RasterizerState RS_Wireframe
{
    FillMode = wireframe;
};

/* DepthStencilState*/
DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None_Depth
{
    DepthEnable = false;
    DepthWriteMask = zero;
};


/* Blend State*/

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;

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

    SrcBlend = One;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = Add;

    SrcBlendAlpha = One;
    DestBlendAlpha = INV_SRC_ALPHA;
    BlendOpAlpha = Add;
};
#endif