float4x4 worldViewProjection : WorldViewProjection;

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
    return float4(v.screenPosition.z / v.screenPosition.w, 0, 0, 0);
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
       AlphaBlendEnable = false;
       VertexShader = compile vs_3_0 vs();
       PixelShader = compile ps_3_0 fs();
   }
}

#endif