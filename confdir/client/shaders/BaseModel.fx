#ifndef MAX_BONES
# define MAX_BONES 58
#endif

float4x4 worldViewProjection : WorldViewProjection;
float4x4 worldViewInverseTranspose;

sampler2D diffuseTexture = sampler_state
{
    minFilter = Linear;
    magFilter = Linear;
};

float4x4 boneMatrix[MAX_BONES];

struct VSout
{
    float4 position     : POSITION;
    float2 texCoord     : TEXCOORD0;
    float3 normal       : TEXCOORD1;
    float4 pos          : TEXCOORD2;
};

struct FSout
{
    float4 diffuse      : COLOR0;
    float4 normalDepth  : COLOR1;
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
    v.texCoord = texCoord;
    normal = mul((float3x3)matTransform, normal);
    v.normal = normalize(mul((float3x3)worldViewInverseTranspose, normal));
    v.pos = v.position;
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

    f.diffuse = tex2D(diffuseTexture, v.texCoord);
    f.normalDepth = float4(encodeNormals(v.normal), v.pos.z / v.pos.w, 1.0);

    return f;
}

technique tech
{
    pass p0
    {
        AlphaBlendEnable = true;
        VertexShader = compile vs_3_0 vs();
        PixelShader = compile ps_3_0 fs();
    }
}
