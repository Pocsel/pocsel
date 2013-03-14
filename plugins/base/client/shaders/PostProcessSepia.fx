float4x4 mvp : WorldViewProjection;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;
float4x4 viewProjectionInverse : ViewProjectionInverse;
float4x4 quadWorldViewProjection;

sampler2D diffuse = sampler_state
{
    minFilter = Point;
    magFilter = Point;
};
sampler2D normalDepth = sampler_state
{
    minFilter = Point;
    magFilter = Point;
};
sampler2D mask = sampler_state
{
    minFilter = Point;
    magFilter = Point;
};

struct VSout
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
};

VSout vs(
	in float4 position : POSITION,
	in float2 texCoord : TEXCOORD0)
{
    VSout vout;
    vout.position = mul(quadWorldViewProjection, position);
    vout.texCoord = texCoord;
    return vout;
}

float4 fs(in VSout v) : COLOR
{
    float4 maskColor = tex2D(mask, v.texCoord * 10);

    float4 color = tex2D(diffuse, v.texCoord);
    color = color; // * 0.75 + color * maskColor * 0.25;
    color.a = 1.0;
    return color;
}

technique tech
{
   pass p0
   {
       AlphaBlendEnable = false;
       VertexShader = compile vs_3_0 vs();
       PixelShader = compile ps_3_0 fs();
   }
}
