float4x4 mvp : WorldViewProjection;
float4x4 viewProjection : ViewProjection;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;
float4x4 viewProjectionInverse : ViewProjectionInverse;
float4x4 screenWorldViewProjection;

float3  lightDirection = float3(0.0, 0.0, 0.0);
float4  lightDiffuseColor = float4(1.0, 1.0, 1.0, 1.0);
float4  lightSpecularColor = float4(0.9, 1.0, 0.8, 1.0);

// TODO:
float   materialSpecularPower = 5.0;

sampler2D normalDepth = sampler_state
{
    minFilter = Point;
    magFilter = Point;
};

struct VSout
{
   float4 position : POSITION;
   float2 texCoord : TEXCOORD0;
};

struct FSout
{
    float4 diffuse : COLOR0;
    float4 specular : COLOR1;
};

VSout vs(in float4 position : POSITION, in float2 texCoord : TEXCOORD0)
{
    VSout vout;
    vout.position = mul(screenWorldViewProjection, position);
    vout.texCoord = texCoord;
    return vout;
}

float3 decodeNormals(float4 enc)
{
    float4 nn = enc*float4(2,2,0,0) + float4(-1,-1,1,-1);
    float l = dot(nn.xyz,-nn.xyw);
    nn.z = l;
    nn.xy *= sqrt(l);
    return nn.xyz * 2 + float3(0,0,-1);
}

float3 decodePosition(float4 enc, float2 coords)
{
    float z = enc.z;
    float x = coords.x * 2 - 1;
    float y = (1 - coords.y) * 2 - 1;
    float4 projPos = float4(x, y, z, 1.0);
    float4 pos = mul(projectionInverse, projPos);
    return pos.xyz / pos.w;
}

FSout fs(in VSout v)
{
    float4 encNormalDepth = tex2D(normalDepth, v.texCoord);
    float3 vWorldNrm = decodeNormals(encNormalDepth);// * 2 - 1;
    float3 vWorldPos = decodePosition(encNormalDepth, v.texCoord);

    float3 vEyeVec = normalize(viewInverse[3].xyz - vWorldPos);
    float4 vDiffuseIntensity = dot(vWorldNrm, mul((float3x3)viewProjection, lightDirection));
    float4 vSpecularIntensity = pow(max(0, dot(vEyeVec, reflect(-lightDirection, vWorldNrm))), materialSpecularPower);

    FSout f;
    f.diffuse = vDiffuseIntensity * lightDiffuseColor;
    f.specular = vSpecularIntensity * lightSpecularColor;
    return f;
}

#ifndef DIRECTX

technique tech_glsl
{
    pass p0
    {
        AlphaBlendEnable = true;
        VertexProgram = compile glslv vs();
        FragmentProgram = compile glslf fs();
    }
}
technique tech
{
    pass p0
    {
        AlphaBlendEnable = true;
        VertexProgram = compile arbvp1 vs();
        FragmentProgram = compile arbfp1 fs();
    }
}

#else

technique tech
{
   pass p0
   {
       AlphaBlendEnable = true;
       AlphaTestEnable = true;
       VertexShader = compile vs_3_0 vs();
       PixelShader = compile ps_3_0 fs();
   }
}

#endif