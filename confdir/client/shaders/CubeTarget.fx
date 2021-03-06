float4x4 mvp : WorldViewProjection;
float time;

sampler2D baseTex = sampler_state
{
    minFilter = Linear;
    magFilter = Linear;
};

struct VSout
{
   float4 position : POSITION;
   float2 texCoord : TEXCOORD0;
   float4 screenPosition : TEXCOORD1;
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

    vout.texCoord = texCoord;
    vout.screenPosition = vout.position;

    return vout;
}

FSout fs(in VSout v)
{
    v.texCoord.x = 0.5 - abs(v.texCoord.x - 0.5);
    v.texCoord.y = 0.5 - abs(v.texCoord.y - 0.5);

    float2 coord = float2((v.texCoord.x - 0.5) * 2.0, (v.texCoord.y - 0.5) * 2.0);
    float tmp = (coord.x * coord.x);
    coord.y = (coord.y * coord.y);

    coord.x = v.texCoord.x + coord.y * cos(time * 3.01) * (0.5 - abs(v.texCoord.x - 0.5));
    coord.y = v.texCoord.y + tmp * cos(time * 3.01) * (0.5 - abs(v.texCoord.y - 0.5));

    FSout f;
    f.color = tex2D(baseTex, coord);
    f.normalDepth = float4(1, 0, v.screenPosition.z / v.screenPosition.w, 0);
    return f;
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
