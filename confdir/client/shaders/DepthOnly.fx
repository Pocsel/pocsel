float4x4 worldViewProjection : WorldViewProjection;

sampler2D diffuse = sampler_state
{
   minFilter = Point;
   magFilter = Point;
};

struct VSout
{
   float4 position : POSITION;
   float4 screenPosition : TEXCOORD0;
};

VSout vs(in float4 position : POSITION)
{
    VSout vout;
    vout.position = mul(worldViewProjection, position);
    vout.screenPosition = vout.position;
    return vout;
}

float4 fs(in VSout v) : COLOR
{
    return float4(screenPosition.z / screenPosition.w, 0, 0, 0);
}

#ifndef DIRECTX

technique tech_glsl
{
    pass p0
    {
        AlphaBlendEnable = true;
        VertexProgram = compile glslv vs();
        FragmentProgram = compile glslf fs();
    }
}
technique tech
{
    pass p0
    {
        AlphaBlendEnable = true;
        VertexProgram = compile arbvp1 vs();
        FragmentProgram = compile arbfp1 fs();
    }
}

#else

technique tech
{
   pass p0
   {
       AlphaBlendEnable = false;
       VertexShader = compile vs_3_0 vs();
       PixelShader = compile ps_3_0 fs();
   }
}

#endif