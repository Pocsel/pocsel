float4x4 mvp : WorldViewProjection;

sampler2D depthBuffer = sampler_state
{
   minFilter = Point;
   magFilter = Point;
};
sampler2D normals = sampler_state
{
   minFilter = Point;
   magFilter = Point;
};
sampler2D colors = sampler_state
{
   minFilter = Point;
   magFilter = Point;
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
#ifdef DIRECTX
    vout.texCoord = texCoord;
#else
    vout.texCoord = float2(texCoord.x, 1.0-texCoord.y);
#endif
    return vout;
}

float4 fs(in VSout v) : COLOR
{
    return tex2D(colors, v.texCoord);
    float2 coords = v.texCoord * 2;
    //coords.y = 2 - coords.y;
    if (coords.y < 1)
    {
        if (coords.x < 1)
            return tex2D(depthBuffer, coords) * float4(1, 1, 1, 1.0);;
        return tex2D(normals, coords - float2(1, 0)) * float4(0, 0, 1, 1.0);;
    }
    else
    {
        coords.y -= 1;
        if (coords.x < 1)
            return tex2D(colors, coords) * float4(1.0, 0, 0, 1.0);

        coords.x = coords.x - 1;
        return tex2D(colors, coords) * float4(0, 1, 0, 1.0);
    }
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