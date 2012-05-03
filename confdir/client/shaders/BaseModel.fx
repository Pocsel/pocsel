#ifndef MAX_BONES
#define MAX_BONES 58
#endif

float4x4 worldViewProjection : WorldViewProjection;
float4x4 world : World;

sampler2D baseTex = sampler_state
{
    minFilter = Linear;
    magFilter = Linear;
};

float4x4 boneMatrix[MAX_BONES];

struct VSout
{
    float4 position      : POSITION;
    float2 texCoord      : TEXCOORD0;
    float4 worldPosition : TEXCOORD1;
    float3 normal        : TEXCOORD2;
};

struct FSout
{
    float4 color    : COLOR0;
    float4 normal   : COLOR1;
    float4 position : COLOR2;
};

VSout vs(
        in float4   position    : POSITION,
        in float3   normal      : NORMAL,
        in float2   texCoord    : TEXCOORD0,
        in float4   weight      : TEXCOORD1,
        in float4   matrixIndex : TEXCOORD2)
{
    float4x4 matTransform = boneMatrix[matrixIndex.x] * weight.x;
    matTransform += boneMatrix[matrixIndex.y] * weight.y;
    matTransform += boneMatrix[matrixIndex.z] * weight.z;
    float finalWeight = 1.0f - (weight.x + weight.y + weight.z);
    matTransform += boneMatrix[matrixIndex.w] * finalWeight;
    //matTransform = mul(world, matTransform);

    VSout vout;
    vout.worldPosition = mul(matTransform, position);
    vout.position = mul(worldViewProjection, vout.worldPosition);
    vout.texCoord = texCoord;
    vout.normal = normalize(mul(world, mul(matTransform, float4(normal, 0.0f))).xyz);
    vout.worldPosition = mul(world, vout.worldPosition);
    return vout;
}

float4 encodeNormals(float3 n)
{
//    float f = n.z * -2 + 1;
//    float g = dot(n, n);
//    float p = sqrt(g + f);
//    return float4(n.xy / p * 0.5 + 0.5, 1, 1);
    return float4(n * 0.5 + 0.5, 1.0);
}

FSout fs(VSout v)
{
    FSout f;
    f.color = tex2D(baseTex, v.texCoord);
    //f.normal = float4(normalize(v.normal), 1.0);
    f.normal = encodeNormals(v.normal);
    f.position = float4(v.worldPosition.xyz, 1.0);
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
        VertexShader = compile vs_3_0 vs();
        PixelShader = compile ps_3_0 fs();
    }
}

#endif
