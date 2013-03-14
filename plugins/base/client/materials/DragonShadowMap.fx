#ifndef MAX_BONES
# define MAX_BONES 58
#endif

float4x4 worldViewProjection : WorldViewProjection;
float4x4 worldViewInverseTranspose;

float4x4 boneMatrix[MAX_BONES];

struct VSout
{
    float4 position     : POSITION;
    float4 pos          : TEXCOORD2;
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

    VSout v;
    v.position = mul(worldViewProjection, mul(matTransform, position));
    v.pos = v.position;
    return v;
}

float4 fs(in VSout v) : COLOR
{
    return float4(v.pos.z / v.pos.w, 0, 0, 1);
}

technique tech
{
    pass p0
    {
        AlphaBlendEnable = false;
        VertexShader = compile vs_3_0 vs();
        PixelShader = compile ps_3_0 fs();
    }
}
