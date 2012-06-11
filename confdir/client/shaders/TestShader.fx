float4x4 mvp : WorldViewProjection;
float4x4 worldViewInverseTranspose;

#ifdef DIRECTX
texture diffuse;

sampler sDiffuse = sampler_state
{
    Texture = <diffuse>;
    MinFilter = Linear;
    MagFilter = Linear;
};

#define diffuse sDiffuse
#else
sampler2D diffuse = sampler_state
{
    MinFilter = Linear;
    MagFilter = Linear;
};
#endif

struct VSout
{
    float4 position : POSITION;
    float4 pos      : TEXCOORD0;
    float3 normal   : TEXCOORD1;
    float2 texCoord : TEXCOORD2;
};

struct FSout
{
    float4 diffuse      : COLOR0;
    float4 normalDepth  : COLOR1;
};

VSout vs(in float4 position : POSITION)
{
    VSout v;
    v.position = mul(mvp, position);
    v.normal = normalize(mul((float3x3)worldViewInverseTranspose, position.xyz));;
    v.pos = v.position;
    v.texCoord = position.xy * 4;
    return v;
}

float2 encodeNormals(float3 n)
{
    float2 enc = normalize(n.xy) * (sqrt(n.z*-0.5+0.5));
    enc = enc*0.5+0.5;
    return float4(enc, 0, 1.0);
}

FSout fs(in VSout v)
{
    FSout f;

    f.diffuse = float4(0.2, 0.2, 0.2, 0) + tex2D(diffuse, v.texCoord);
    f.normalDepth = float4(encodeNormals(v.normal), v.pos.z / v.pos.w, 1.0);

    return f;
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
        AlphaBlendEnable = false;
        VertexShader = compile vs_2_0 vs();
        PixelShader = compile ps_2_0 fs();
    }
}

#endif