float4x4 worldViewProjection : WorldViewProjection;
float4x4 world : World;

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
   float3 worldPosition : TEXCOORD2;
};

struct FSout
{
    float4 diffuse : COLOR0;
    float4 normals : COLOR1;
    float4 depth : COLOR2;
};

VSout vs(in float4 position : POSITION, in float2 texCoord : TEXCOORD0, in float3 normals : NORMAL)
{
    VSout vout;
    vout.position = mul(worldViewProjection, position);
    vout.texCoord = texCoord;
    vout.normals = normalize(mul(world, float4(normals, 0.0)).xyz);
    vout.worldPosition = mul(world, position);
    return vout;
}

FSout fs(in VSout v)
{
    FSout f;

    f.diffuse = tex2D(diffuse, v.texCoord);
    f.normals = float4(v.normals * 0.5 + 0.5, 1.0);
    f.depth = float4(v.worldPosition.x, v.worldPosition.y, v.worldPosition.z, 1.0);

    return f;
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