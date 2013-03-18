float4x4 mvp : WorldViewProjection;
float4 colorParam;

struct VSout
{
   float4 position : POSITION;
   //float2 texCoord : TEXCOORD0;
   //float4 screenPosition : TEXCOORD1;
};

struct FSout
{
    float4 color : COLOR0;
    float4 normalDepth : COLOR1;
};

VSout vs(in float4 position : POSITION, in float2 texCoord : TEXCOORD0)
{
    VSout vout;
    vout.position = mul(mvp, position);

    //vout.texCoord = texCoord;
    //vout.screenPosition = vout.position;

    return vout;
}

FSout fs(in VSout v)
{
    FSout f;
    f.color = float4(colorParam.rgb, 1);
    f.normalDepth = float4(0, 0, 0, 1);
    return f;
}

technique tech
{
    pass p0
    {
        AlphaBlendEnable = false;
        VertexShader = compile vs_2_0 vs();
        PixelShader = compile ps_2_0 fs();
    }
}
