texture RTTexture;

sampler2D s0 = sampler_state{
    Texture = <RTTexture>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
float uTime;
float uDist;
float uState;
float uRatio;


float2  uTexSize    = {1280.0f, 720.0f};
float   iPixel      = 4.0f;
float   iAmplitude  = 1.2f;
float   iPeriod     = 12.0f;

float3 FireColor    = { 240.0f / 256.0f,  55.0f / 256.0f ,  16.0f / 256.0f};
float3 FireInColor  = { 200.0f / 256.0f,   9.0f / 256.0f ,   2.0f / 256.0f};
float3 IceColor     = { 122.0f / 256.0f, 164.0f / 256.0f , 230.0f / 256.0f};
float3 IceInColor   = {  82.0f / 256.0f,  91.0f / 256.0f , 206.0f / 256.0f};


float3 FireOLColor = { 240.0f / 256.0f,  105.0f / 256.0f ,   11.0f / 256.0f };
float3 IceOLColor  = { 102.0f / 256.0f,  253.0f / 256.0f ,  235.0f / 256.0f };

float4 GetColor(float4 _color, float _ratio)
{
    float3 ResultColor, ResultInColor;
    ResultColor     = (uState * FireColor + (1 - uState) * IceColor);
    ResultInColor   = (uState * FireInColor + (1 - uState) * IceInColor);
    _color.rgb = (1 - _ratio) * ResultColor + _ratio * ResultInColor;
    return _color;
}
float4 OutLineColor()
{
    float4 ResultColor = float4(1,1,1,1);
    ResultColor.rgb = (uState * FireOLColor + (1 - uState) * IceOLColor);
    return ResultColor;
}

float Value(float3 rgb) {
    return max(max(rgb.r, rgb.g), rgb.b);
}
float HeatDistortionOffset(float _y, float _size, float _T, float _phase) {
    _y = floor((_y * _size) / iPixel) / (_size / iPixel);
    float _offset = 4*((_y + _phase*_T) / _T - floor((_y + _phase*_T) / _T));
    _offset = abs(_offset - 2)-1;
    return round(_offset);
}
float BoundaryOffset(float _y, float _size, float _T, float _phase) {
    _y = floor((_y * _size) / iPixel) / (_size / iPixel);
    float _offset = 4 * ((_y + _phase * _T) / _T - floor((_y + _phase * _T) / _T));
    _offset = abs(_offset - 2) - 1;
    return round(_offset);
}

float3 ApplyWarmth(float3 color, float warmth)
{
    float3x3 matWarm = {
        1.0 + 0.1 * warmth,  -0.05 * warmth,       0.0,
       -0.02 * warmth,        1.0 + 0.05 * warmth, 0.0,
        0.0,               -0.05 * warmth,       1.0 - 0.1 * warmth
    };
    //return (mul(matWarm, color));
    return saturate(mul(matWarm, color));
}

struct VSIn  { float4 pos : POSITION; float2 uv : TEXCOORD0; };
struct VSOut { float4 pos : POSITION; float2 uv : TEXCOORD0; };

VSOut  VS(VSIn i) { 
    VSOut o; 
    o.pos = i.pos; 
    o.uv = i.uv; 
    return o; 
}

float4 FIREPS(VSOut i) : COLOR{

    float _dx = iPixel / uTexSize.x * HeatDistortionOffset(i.uv.y + uDist, uTexSize.y,  iPeriod * iPixel / uTexSize.y, uTime);
    float4 _color = tex2D(s0, i.uv + float2(_dx, 0));
    float _v = Value(_color.rgb);
    _color.rgb = ApplyWarmth(_color.rgb, 3.0f * (1.5f - pow(_v, 1.5)));
    return _color;
}

float4 ICEPS(VSOut i) : COLOR{
    float4 _color = tex2D(s0, i.uv);

    float _v = Value(_color.rgb);
    _color.rgb = ApplyWarmth(_color.rgb, -2.5f * (2.0f - pow(_v,4.0)));

    return _color;
}

float4 BoundaryPS(VSOut i) : COLOR{
    float _dy = 2 * iPixel * sin((i.uv.x) * 3 * 6.28);


    float _amplitude =2 * iPixel;
    float _T         = 10;
    float _d = (4 * (i.uv.x) * _T);
    float _t = 4.0 * _amplitude * (_d - floor(_d));

    float _iceOffset    =  abs(2.0 * _amplitude - _t) - _amplitude;
    float _fireOffset = _amplitude * cos(6.28 * ((i.uv.x ) * _T + uTime));

    _dy += (1 - uState) * _iceOffset + uState * _fireOffset;

    _dy = _dy / uTexSize.y;
    _dy = floor(_dy * uTexSize.y / iPixel) / (uTexSize.y / iPixel);


    float4 _color    = tex2D(s0, i.uv + float2(0, _dy));
    float4 _refColor = tex2D(s0, i.uv + float2(0, _dy - (1/ uTexSize.y)*iPixel));
    if (_color.r != _refColor.r)
        return OutLineColor();
    return GetColor(_color, _color.g);
}


float4 BoundaryBlockPS(VSOut i) : COLOR{
    float _dx = 1 * iPixel * sin((i.uv.y + uDist) * 3 * 6.28);


    float _amplitude = 1 * iPixel;
    float _T = 10;
    float _d = (4 * (i.uv.y + uDist) * _T);
    float _t = 4.0 * _amplitude * (_d - floor(_d));

    float _iceOffset = abs(2.0 * _amplitude - _t) - _amplitude;
    float _fireOffset = _amplitude * cos(6.28 * ((i.uv.y + uDist) * _T + uTime));

    _dx += (1 - uState) * _iceOffset + uState * _fireOffset;

    _dx = _dx / uTexSize.x;
    _dx = floor(_dx * uTexSize.x / iPixel) / (uTexSize.x / iPixel);


    float4 _color = tex2D(s0, i.uv + float2(_dx, 0));
    float4 _refColor = tex2D(s0, i.uv + float2(_dx - (1 / uTexSize.x) * iPixel, 0));
    if (_color.r != _refColor.r)
        return OutLineColor();
    return GetColor(_color, _color.g);
}


float4 FadePS1(VSOut i) : COLOR{

    float4 _color = {0.0, 0.0, 0.0, 0.0};
    _color.a = uRatio;
    return _color;
}
float4 FadePS2(VSOut i) : COLOR{
    float2 _uv = i.uv;
    _uv = saturate(0.5f + uRatio * (_uv - 0.5f));
    //_uv = saturate(uRatio * (_uv));

    float4 _color = tex2D(s0, _uv);
    _color.a = 1-_color.r;
    return _color;
}

float4 FadePS3(VSOut i) : COLOR{

    float4 _color = {255.0, 255.0, 255.0, 255.0};
    _color.a = 1-uRatio;
    return _color;
}

technique T_Pass{
    pass P0 {
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 FIREPS(); 
    }
    pass P1 {
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 ICEPS();
    }
    pass P2 {
        VertexShader = compile vs_3_0 VS();
        PixelShader  = compile ps_3_0 BoundaryPS();
    }
    pass P3 {
        VertexShader = compile vs_3_0 VS();
        PixelShader = compile ps_3_0 BoundaryBlockPS();
    }
    pass P4 {
        VertexShader = compile vs_3_0 VS();
        PixelShader = compile ps_3_0 FadePS1();
    }
    pass P5 {
        VertexShader = compile vs_3_0 VS();
        PixelShader = compile ps_3_0 FadePS2();
    }
    pass P6 {
        VertexShader = compile vs_3_0 VS();
        PixelShader = compile ps_3_0 FadePS3();
    }
}