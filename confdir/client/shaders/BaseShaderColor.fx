float4x4 mvp : WorldViewProjection;

struct VSout
{
    float4 position : POSITION;
    float4 color : COLOR;
};

VSout vs(in float4 position : POSITION, in float4 color : COLOR)
{
    VSout vout;
    vout.position = mul(mvp, position);
    vout.color = color;
    return vout;
}

float4 fs(in VSout v) : COLOR
{
    return v.color;
}

technique tech
{
    pass p0
    {
        AlphaBlendEnable = true;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        ZEnable = false;
        VertexShader = compile vs_2_0 vs();
        PixelShader = compile ps_2_0 fs();
    }
}
