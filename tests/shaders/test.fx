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
   float4 pos : TEXCOORD3;
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
    vout.pos = vout.position;
    return vout;
}

float4 encodeNormals(float3 n)
{
    float2 enc = normalize(n.xy) * (sqrt(-n.z*0.5+0.5));
    enc = enc*0.5+0.5;
    return float4(enc, 0, 1.0);
}

FSout fs(in VSout v)
{
    FSout f;

    f.diffuse = tex2D(diffuse, v.texCoord);
    f.normals = encodeNormals(v.normals);// float4(v.normals * 0.5 + 0.5, 1.0);
    f.depth = float4(v.pos.z / v.pos.w, v.worldPosition.x, v.worldPosition.y, v.worldPosition.z);

    return f;
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