float4x4 worldViewProjection : WorldViewProjection;
float4x4 viewProjection : ViewProjection;
float4x4 world : World;
float4x4 worldView : WorldView;
float4x4 viewInverse : ViewInverse;
float4x4 worldViewInverseTranspose;

float3 fogColor = float3(0.8, 0.8, 0.9);
float fogEnd = 750.0f;

// Water
float time = 0;
float displacement = 4.0;

#ifdef DIRECTX
sampler2D cubeTexture = sampler_state
{
    MinFilter = Linear;
    MagFilter = Point;
    MipFilter = Linear;
};
#else
sampler2D cubeTexture = sampler_state
{
    minFilter = LinearMipMapLinear;
    magFilter = Nearest;
};
#endif

struct VSout
{
    float4 position      : POSITION;
    float2 texCoord      : TEXCOORD0;
    float3 normal       : TEXCOORD1;
    float4 pos           : TEXCOORD2;
};

struct FSout
{
    float4 diffuse : COLOR0;
    float4 normalsDepth : COLOR1;
};

VSout vs(in float4 position : POSITION, in float3 normal : NORMAL, in float2 texCoord : TEXCOORD0)
{
    VSout v;

    v.texCoord = texCoord;
    v.position = mul(worldViewProjection, position);
    v.normal = normalize(mul(world, float4(normal, 0.0)).xyz);
    v.pos = mul(worldViewProjection, position);

    return v;
}

float2 encodeNormals(float3 n)
{
   float f = n.z * 2 + 1;
   float p = sqrt(dot(n, n) + f);
   return n.xy / p * 0.5 + 0.5;
}

FSout fs(in VSout v)
{
    FSout f;

    f.diffuse = tex2D(cubeTexture, v.texCoord);
    f.normalsDepth = float4(encodeNormals(v.normal), v.pos.z / v.pos.w, 1.0);

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
        VertexShader = compile vs_2_0 vs();
        PixelShader = compile ps_2_0 fs();
   }
}

#endif