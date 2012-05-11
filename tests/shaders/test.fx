float4x4 worldViewProjection : WorldViewProjection;
float4x4 world : World;
float4x4 worldViewInverseTranspose;

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
   float4 pos : TEXCOORD2;
};

struct FSout
{
    float4 diffuse : COLOR0;
    float4 normalsDepth : COLOR1;
    //float4 depth : COLOR2;
};

VSout vs(in float4 position : POSITION/*, in float3 normals : NORMAL, in float2 texCoord : TEXCOORD0*/)
{
    VSout vout;
    vout.position = mul(worldViewProjection, position);
    vout.texCoord = float2(0,0);
    vout.normals = normalize(mul((float3x3)worldViewInverseTranspose, position.xyz));
    vout.pos = vout.position;
    return vout;
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

    f.diffuse = float4(1, 1, 1, 1); //tex2D(diffuse, v.texCoord);
    f.normalsDepth = float4(encodeNormals(v.normals), v.pos.z / v.pos.w, 1.0);// float4(v.normals * 0.5 + 0.5, 1.0);
    //f.depth = float4(v.pos.z / v.pos.w, v.worldPosition.x, v.worldPosition.y, v.worldPosition.z);

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
        AlphaTestEnable = false;
        VertexShader = compile vs_3_0 vs();
        PixelShader = compile ps_3_0 fs();
    }
}

#endif