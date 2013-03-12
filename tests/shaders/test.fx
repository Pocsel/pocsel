float4x4 worldViewProjection : WorldViewProjection;
float4x4 world : World;
float4x4 worldViewInverseTranspose;

sampler2D diffuse = sampler_state
{
   minFilter = Linear;
   magFilter = Linear;
};

float4 test;

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
    vout.normals = normals;//normalize(mul((float3x3)worldViewInverseTranspose, position.xyz));
    vout.pos = vout.position * test;
    return vout;
}

float4 fs(in VSout v) : COLOR0
{
    //return tex2D(diffuse, v.texCoord);
    return tex2D(diffuse, v.texCoord) * test;
}

#ifndef DIRECTX

technique tech_glsl
{
    pass p0
    {
        AlphaBlendEnable = false;
        VertexProgram = compile glslv vs();
        FragmentProgram = compile glslf fs();
    }
}
technique tech
{
    pass p0
    {
        AlphaBlendEnable = false;
        VertexProgram = compile arbvp1 vs();
        FragmentProgram = compile arbfp1 fs();
    }
}

#else

technique tech
{
    pass p0
    {
        //AlphaBlendEnable = false;
        //AlphaTestEnable = false;
        VertexShader = compile vs_3_0 vs();
        PixelShader = compile ps_3_0 fs();
    }
}

#endif
