float4x4 mvp : WorldViewProjection;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;
float4x4 viewProjectionInverse : ViewProjectionInverse;
float4x4 quadWorldViewProjection;

float3 lightPos = float3(0, 0, 0);
float3 lightDiffuse = float3(1.0, 1.0, 1.0);
float3 lightSpecular = float3(0.9, 1.0, 0.8);

sampler2D normalsDepth = sampler_state
{
   minFilter = Point;
   magFilter = Point;
};
//sampler2D depthBuffer = sampler_state
//{
//   minFilter = Point;
//   magFilter = Point;
//};
sampler2D diffuse = sampler_state
{
   minFilter = Point;
   magFilter = Point;
};

struct VSout
{
   float4 position : POSITION;
   float2 texCoord : TEXCOORD0;
   float4 pos : TEXCOORD1;
};

VSout vs(in float4 position : POSITION, in float2 texCoord : TEXCOORD0)
{
    VSout vout;
    vout.position = mul(quadWorldViewProjection, position);
#ifdef DIRECTX
    vout.texCoord = texCoord;
#else
    vout.texCoord = float2(texCoord.x, 1-texCoord.y);
#endif
    vout.pos = vout.position;
    return vout;
}

float3 decodeNormals(float4 enc)
{
    float3 normal = float3(-enc.xy * enc.xy + enc.xy, -1);
    float f = dot(normal, float3(1, 1, 0.25));
    float m = sqrt(f);
    normal.xy = (enc.xy * 8 - 4) * m;
    normal.z = -(1 - 8 * f);
    return normal;
}

float3 decodePosition(float4 enc, float2 coords)
{
#ifdef DIRECTX
    float z = enc.z;
#else
    float z = enc.z * -0.5 + 0.5;
#endif
    //return float4(z, z, z, 1.0);
    //return float3(coords.x, coords.y, z);
    //return tex2D(depthBuffer, coords).yzw;
    float x = coords.x * 2 - 1;
    float y = (1 - coords.y) * 2 - 1;
    //return float3(abs(x), abs(y), z);
    float4 projPos = float4(x, y, 1-z, 1.0);
    float4 pos = mul(viewProjectionInverse, projPos);
    return pos.xyz / pos.w;
}

float4 fs(in VSout v) : COLOR
{
    float2 coords = v.texCoord * 2;
    if (coords.y < 1)
    {
        if (coords.x < 1)
        {
            float4 encNormalDepth = tex2D(normalsDepth, coords);
            return float4(decodePosition(encNormalDepth, coords)*20, 1.0);
        }
        coords.x = coords.x - 1;
        float4 encNormalDepth = tex2D(normalsDepth, coords);
        return float4(decodeNormals(encNormalDepth) * 0.5 + 0.5, 1.0); //tex2D(normals, coords);
    }
    else
    {
        coords.y -= 1;
        if (coords.x < 1)
            return tex2D(diffuse, coords);
        coords.x = coords.x - 1;

        float4 encNormalDepth = tex2D(normalsDepth, coords);
        float3 diffuseColor = tex2D(diffuse, coords).rgb;
        float3 vWorldNrm = decodeNormals(encNormalDepth);// * 2 - 1;
        float3 vWorldPos = decodePosition(encNormalDepth, coords);

        float3 vLightDir = normalize(lightPos - vWorldPos);
        float3 vEyeVec = normalize(viewInverse[3].xyz - vWorldPos);
        float3 vDiffuseIntensity = dot(vWorldNrm, vLightDir);
        float3 vSpecularIntensity = pow(max(0, dot(vEyeVec, reflect(-vLightDir, vWorldNrm))), 5);

        float4 color;
        color.rgb = (vDiffuseIntensity * lightDiffuse) * diffuseColor
             + vSpecularIntensity * lightSpecular.xyz;
        color.a = 1;
        return color;
    }
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
       VertexShader = compile vs_3_0 vs();
       PixelShader = compile ps_3_0 fs();
   }
}

#endif
