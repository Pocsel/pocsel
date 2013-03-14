float4x4 worldViewProjection : WorldViewProjection;
float4x4 worldViewInverseTranspose;

sampler2D cubeTexture = sampler_state
{
    MinFilter = Linear;
    MagFilter = Point;
    MipFilter = Linear;
};

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
	in float4 position : POSITION,
	in float normalTexCoord : TEXCOORD0)
{
    normalTexCoord /= 256.0;
    float3 normal = floor(frac(float3(normalTexCoord * 4, normalTexCoord * 16, normalTexCoord * 64)) * 4);
    normal = normal - 1.0;

    float2 texCoord = floor(frac(float2(normalTexCoord, normalTexCoord * 2)) * 2);

    VSout v;

    v.texCoord = texCoord;
    v.position = mul(worldViewProjection, position);
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
    float4 diffuse = tex2D(cubeTexture, v.texCoord);
    float specularPower = diffuse.r * 0.299 + diffuse.g * 0.587 + diffuse.b * 0.114;
    specularPower = specularPower*specularPower;

    FSout f;

    f.diffuse = float4(diffuse.rgb, 1);
    f.normalDepth = float4(encodeNormals(v.normal), 1 - v.pos.z / v.pos.w, specularPower);

    return f;
}

technique tech
{
   pass p0
   {
        AlphaBlendEnable = false;
        VertexShader = compile vs_2_0 vs();
        PixelShader = compile ps_2_0 fs();
   }
}
