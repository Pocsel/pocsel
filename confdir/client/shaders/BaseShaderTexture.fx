float4x4 mvp : WorldViewProjection;

sampler2D baseTex = sampler_state
{
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

struct VSout
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

VSout vs(in float4 position : POSITION, in float2 texCoord : TEXCOORD0)
{
    VSout vout;
    vout.position = mul(mvp, position);
    vout.texCoord = texCoord;
    return vout;
}

float4 fs(in VSout v) : COLOR
{
    return tex2D(baseTex, v.texCoord).rrrr;
}

technique tech
{
   pass p0
   {
        AlphaBlendEnable = true;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        VertexShader = compile vs_2_0 vs();
        PixelShader = compile ps_2_0 fs();
   }
}
