float4x4 worldViewProjection : WorldViewProjection;

struct VSout
{
    float4 position     : POSITION;
    float4 pos          : TEXCOORD0;
};

struct FSout
{
    float4 diffuse      : COLOR0;
    float4 normalDepth  : COLOR1;
};

VSout vs(in float4 position : POSITION)
{
    VSout v;
    v.position = mul(worldViewProjection, position);
    v.pos = position;
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
    float4 diffuse = normalize(v.pos);
    diffuse.a = 1.0;
    float specularPower = diffuse.r * 0.299 + diffuse.g * 0.587 + diffuse.b * 0.114;
    specularPower = specularPower*specularPower;

    FSout f;

    f.diffuse = diffuse;
    f.normalDepth = float4(encodeNormals(normalize(v.pos)), v.pos.z / v.pos.w, specularPower);

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
