float4x4 mvp : WorldViewProjection;

struct VSout
{
   float4 position : POSITION;
   float4 color : COLOR;
};

VSout vs(in float4 position : POSITION, in float4 color : COLOR)
{
   VSout vout;
   vout.position = mul(mvp, position);
   vout.color = color;
   return vout;
}

float4 fs(in VSout v) : COLOR
{
   return v.color;
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