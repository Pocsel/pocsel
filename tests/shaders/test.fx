float4x4 worldViewProjection : WorldViewProjection;
float4x4 world : World;
float4x4 worldViewInverseTranspose;

sampler2D diffuse = sampler_state
{
   minFilter = Linear;
   magFilter = Linear;
};

struct VSout
{
   float4 position : POSITION;
   float2 texCoord : TEXCOORD0;
   float3 normals : TEXCOORD1;
   float4 pos : TEXCOORD2;
};

VSout vs(in float4 position : POSITION, in float3 normals : NORMAL, in float2 texCoord : TEXCOORD0)
{
    VSout vout;
    vout.position = mul(worldViewProjection, position);
    vout.texCoord = texCoord;
    vout.normals = mul((float3x3)worldViewInverseTranspose, normals);
    vout.pos = vout.position;
    return vout;
}

float4 fs(in VSout v) : COLOR0
{
    return float4(v.normals, 1);
}

technique tech
{
    pass p0
    {
        AlphaBlendEnable = false;
        AlphaTestEnable = false;
        VertexShader = compile vs_3_0 vs();
        PixelShader = compile ps_3_0 fs();
    }
}
