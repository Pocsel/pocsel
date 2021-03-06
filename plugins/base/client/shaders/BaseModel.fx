#ifndef MAX_BONES
# define MAX_BONES 58
#endif

float4x4 worldViewProjection : WorldViewProjection;
float4x4 worldViewInverseTranspose : WorldViewInverseTranspose;

sampler2D diffuseTexture = sampler_state
{
    minFilter = Linear;
    magFilter = Linear;
};

float4x4 boneMatrices[MAX_BONES];
float updateFlag = 0.0;

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
    in float4 position    : POSITION,
    in float3 normal      : NORMAL,
    in float2 texCoord    : TEXCOORD0,
    in float4 weight      : TEXCOORD1,
    in float4 matrixIndex : TEXCOORD2)
{
    float4x4 matTransform = boneMatrices[matrixIndex.x] * weight.x;
    matTransform += boneMatrices[matrixIndex.y] * weight.y;
    matTransform += boneMatrices[matrixIndex.z] * weight.z;
    float finalWeight = 1.0f - (weight.x + weight.y + weight.z);
    matTransform += boneMatrices[matrixIndex.w] * finalWeight;

    VSout v;
    v.position = mul(worldViewProjection, mul(matTransform, position));
    v.texCoord = texCoord;
    normal = mul((float3x3)matTransform, normalize(normal));
    v.normal = normalize(mul((float3x3)worldViewInverseTranspose, normal));
    v.pos = v.position;
    return v;
}

float2 encodeNormals(float3 n)
{
    float2 enc = normalize(n.xy) * (sqrt(n.z*-0.5+0.5));
    enc = enc*0.5+0.5;
    return enc;
}

FSout fs(in VSout v)
{
    float4 diffuse = tex2D(diffuseTexture, v.texCoord);
    float specularPower = diffuse.r * 0.299 + diffuse.g * 0.587 + diffuse.b * 0.114;
    specularPower = specularPower*specularPower;

    FSout f;

    f.diffuse = float4(diffuse.rgb, 1);
    f.normalDepth = float4(encodeNormals(v.normal), 1 - v.pos.z / v.pos.w, specularPower);
    f.diffuse.rgb = f.diffuse.rgb + float3(updateFlag, updateFlag, updateFlag);

    return f;
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
