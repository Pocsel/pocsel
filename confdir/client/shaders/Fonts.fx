float4x4 modelViewProjectionMatrix : WorldViewProjection;
float4 color = float4(0, 0, 1, 1);
float multZ = 0;

sampler2D fontTex = sampler_state
{
    minFilter = Linear;
    magFilter = Linear;
};

struct VSout
{
	float4 pos : POSITION;
	float2 texCoord : TEXCOORD0;
};

VSout vs(
	in float4 pos : POSITION,
    in float2 texCoord : TEXCOORD0)
{
    VSout vout;
    vout.texCoord = texCoord;
	pos.z = pos.z * multZ;
	vout.pos = mul(modelViewProjectionMatrix, pos);
	return vout;
}

float4 fs(in VSout vin) : COLOR
{
    float4 c = tex2D(fontTex, vin.texCoord);
    c.rgb = c.r * color.rgb;
    c.a = c.a * color.a;
    return c;
}

technique tech
{
   pass p0
   {
       AlphaBlendEnable = true;
       VertexShader = compile vs_2_0 vs();
       PixelShader = compile ps_2_0 fs();
   }
}
