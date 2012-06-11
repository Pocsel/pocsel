float4x4 modelViewProjectionMatrix : WorldViewProjection;
float4 color = {0, 0, 1, 1};
float multZ = 0;

#ifdef DIRECTX
texture fontTex;

sampler sFontTex = sampler_state
{
    Texture = <fontTex>;
    minFilter = Linear;
    magFilter = Linear;
};

#define fontTex sFontTex

#else
sampler2D fontTex = sampler_state
{
    minFilter = Linear;
    magFilter = Linear;
};
#endif

float4 vs(in float4 pos : POSITION,
          in float2 iTexCoord : TEXCOORD0,
          out float2 oTexCoord : TEXCOORD0) : POSITION
{
    pos.z = pos.z * multZ;
    oTexCoord = iTexCoord;
    return mul(modelViewProjectionMatrix, pos);
}

float4 fs(in float2 texCoord : TEXCOORD0) : COLOR
{
    float4 c = tex2D(fontTex, texCoord);
    c.rgb = c.r * color.rgb;
    c.a = c.a * color.a;
    return c;
}

#ifndef DIRECTX

technique tech_glsl
{
    pass p0
    {
        AlphaBlendEnable = true;
        AlphaTestEnable = true;
        BlendFunc = int2(SrcAlpha, InvSrcAlpha);
        VertexProgram = compile glslv vs();
        FragmentProgram = compile glslf fs();
    }
}
technique tech
{
    pass p0
    {
        AlphaBlendEnable = true;
        AlphaTestEnable = true;
        BlendFunc = int2(SrcAlpha, InvSrcAlpha);
        VertexProgram = compile arbvp1 vs();
        FragmentProgram = compile arbfp1 fs();
    }
}

#else

technique tech
{
   pass p0
   {
       AlphaBlendEnable = true;
       VertexShader = compile vs_2_0 vs();
       PixelShader = compile ps_2_0 fs();
   }
}

#endif
