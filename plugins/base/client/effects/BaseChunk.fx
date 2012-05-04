float4x4 worldViewProjection : WorldViewProjection;
float4x4 viewProjection : ViewProjection;
float4x4 world : World;
float4x4 worldView : WorldView;
float4x4 viewInverse : ViewInverse;

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
    float3 worldPosition : TEXCOORD1;
    float3 normal        : TEXCOORD2;
    float4 pos           : TEXCOORD3;
};

struct FSout
{
    float4 color    : COLOR0;
    float4 normal   : COLOR1;
    float4 position : COLOR2;
};

VSout vs(in float4 position : POSITION, in float3 normal : NORMAL, in float2 texCoord : TEXCOORD0)
{
    VSout v;

    v.texCoord = texCoord;
    v.worldPosition = mul(world, position);
    v.position = mul(worldViewProjection, position);
    v.normal = normalize(mul(world, float4(normal, 0.0)).xyz);
    v.pos = mul(worldViewProjection, position);

    return v;
}

float4 encodeNormals(float3 n)
{
//    float f = n.z * -2 + 1;
//    float g = dot(n, n);
//    float p = sqrt(g + f);
//    return float4(n.xy / p * 0.5 + 0.5, 1, 1);
    return float4(n * 0.5 + 0.5, 1.0);
}

FSout fs(in VSout v)
{
    FSout f;

    f.color = tex2D(cubeTexture, v.texCoord);
    f.normal = encodeNormals(v.normal);
    f.position = float4(v.worldPosition.x, v.worldPosition.y, v.worldPosition.z, 1.0); //-v.pos.z / 200);

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
        AlphaBlendEnable = false;
        VertexShader = compile vs_2_0 vs();
        PixelShader = compile ps_2_0 fs();
   }
}

#endif