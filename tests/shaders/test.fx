float4x4 mvp : WorldViewProjection;

sampler2D diffuse = sampler_state
{
   minFilter = Linear;
   magFilter = Linear;
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
    return tex2D(diffuse, v.texCoord);
}

#ifndef DIRECTX

technique tech_glsl
{
    pass p0
    {
        VertexProgram = compile glslv vs();
        FragmentProgram = compile glslf fs();
    }
}
technique tech
{
    pass p0
    {
        VertexProgram = compile arbvp1 vs();
        FragmentProgram = compile arbfp1 fs();
    }
}

#else

technique tech
{
   pass p0
   {
       VertexShader = compile vs_2_0 vs();
       PixelShader = compile ps_2_0 fs();
   }
}

#endif