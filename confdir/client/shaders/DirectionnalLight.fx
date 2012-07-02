float4x4 mvp : WorldViewProjection;
float4x4 view : View;
float4x4 viewProjection : ViewProjection;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;
float4x4 viewProjectionInverse : ViewProjectionInverse;
float4x4 worldView : WorldView;
float4x4 screenWorldViewProjection;

float3   lightAmbientColor = float3(0.2f, 0.2f, 0.2f);
float3   lightDirection = float3(0.0, 0.0, 0.0);
float3   lightDiffuseColor = float3(1.0, 1.0, 1.0);
float3   lightSpecularColor = float3(0.9, 1.0, 0.8);
float4x4 lightViewProjection;

// TODO:
float   materialShininess = 5.0;

#ifdef DIRECTX
texture lightShadowMap;
texture normalDepth;

sampler sLightShadowMap = sampler_state
{
    Texture = <lightShadowMap>;
    minFilter = Point;
    magFilter = Point;
};

sampler sNormalDepth = sampler_state
{
    Texture = <normalDepth>;
    minFilter = Point;
    magFilter = Point;
};

#define normalDepth sNormalDepth
#define lightShadowMap sLightShadowMap
#else
sampler2D normalDepth = sampler_state
{
    minFilter = Point;
    magFilter = Point;
};

sampler2D lightShadowMap = sampler_state
{
    minFilter = Point;
    magFilter = Point;
};
#endif

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
#ifdef DIRECTX
    vout.texCoord = texCoord;
#else
    vout.texCoord = float2(texCoord.x, 1-texCoord.y);
#endif
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
    float z = 1-enc.z;
    float x = coords.x * 2 - 1;
#ifdef DIRECTX
    float y = (1 - coords.y) * 2 - 1;
#else
    float y = coords.y * 2 - 1;
#endif
    float4 projPos = float4(x, y, z, 1.0);
    float4 pos = mul(projectionInverse, projPos);
    return pos.xyz / pos.w;
}

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
    float3 diff = decodePosition(tex2D(normalDepth, tcoord + uv), tcoord + uv) - p;
    const float3 v = normalize(diff);
    const float d = length(diff)*1.0;
    return max(0.0,dot(cnorm,v)-0.001)*(1.0/(1.0+d))*2;
}

float screenSpaceAmbientOcclusion(float2 texCoord, float3 viewPosition, float3 viewNormal)
{
    // SSAO
    const float2 vec[4] = {float2(1,0),float2(-1,0),float2(0,1),float2(0,-1)};

    float ao = 0.0f;
    float rad = 1.5f / viewPosition.z;

    int iterations = 4;
    for (int j = 0; j < iterations; ++j)
    {
        float2 coord1 = reflect(vec[j], normalize(float2(0, 1))) * rad;
        float2 coord2 = float2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707);

        ao += doAmbientOcclusion(texCoord, coord1*0.25, viewPosition, viewNormal);
        ao += doAmbientOcclusion(texCoord, coord2*0.5, viewPosition, viewNormal);
        ao += doAmbientOcclusion(texCoord, coord1*0.75, viewPosition, viewNormal);
        ao += doAmbientOcclusion(texCoord, coord2, viewPosition, viewNormal);
    }
    ao /= (float)iterations*4.0;
    //ao = 1 - ao;
    return ao;
}

float calculateShadowMap(float3 viewPosition)
{
    float4 lightingPosition = mul(mul(view, lightViewProjection), float4(viewPosition, 1.0));
    float2 shadowTexCoord = 0.5f * lightingPosition.xy / lightingPosition.w + float2(0.5, 0.5);
    shadowTexCoord.y = 1.0f - shadowTexCoord.y;

    float shadowDepth = tex2D(lightShadowMap, shadowTexCoord).r;
    float ourDepth = lightingPosition.z / lightingPosition.w;
    if (shadowDepth + 0.000006 < ourDepth)
        return 0.3;
    return 1.0;
}

FSout fs(in VSout v)
{
    float3 viewLightDirection = normalize(mul((float3x3)worldView, -lightDirection));

    float4 encNormalDepth = tex2D(normalDepth, v.texCoord);
    float3 viewNormal = decodeNormals(encNormalDepth);// * 2 - 1;
    float3 viewPosition = decodePosition(encNormalDepth, v.texCoord);
    float specularPower = encNormalDepth.w;

    float NdL = max(0, dot(viewNormal, viewLightDirection));

    float3 viewDirection = normalize(viewPosition);
    float3 reflection = reflect(viewLightDirection, viewNormal);
    //float specular = pow(max(0.0, dot(reflection, viewDirection)), materialShininess);
    float specular = pow(max(0.0, dot(reflection, viewDirection)), (1.0 - specularPower) * 25);

    float ao = 0;//screenSpaceAmbientOcclusion(v.texCoord, viewPosition, viewNormal);
    float shadowMap = 1.0; //calculateShadowMap(viewPosition);

    FSout f;
    f.diffuse.rgb = (NdL * lightDiffuseColor - ao) * shadowMap;
    f.diffuse.rgb += lightAmbientColor;
    f.specular.rgb = (specular * lightSpecularColor) * shadowMap * specularPower;
    f.diffuse.a = 1.0;
    f.specular.a = 1.0;
    return f;
}

#ifndef DIRECTX

technique tech_glsl
{
    pass p0
    {
        AlphaBlendEnable = true;
        AlphaTestEnable = true;
        AlphaRef = 0;
        SrcBlend = 1;
        DestBlend = 1;
        ZEnable = false;
        VertexProgram = compile glslv vs();
        FragmentProgram = compile glslf fs();
    }
}
technique tech
{
    pass p0
    {
        AlphaBlendEnable = true;
        AlphaTestEnable = true;
        AlphaRef = 0;
        SrcBlend = 1;
        DestBlend = 1;
        ZEnable = false;
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
        AlphaRef = 0;
        SrcBlend = One;
        DestBlend = One;
        ZEnable = false;
        VertexShader = compile vs_3_0 vs();
        PixelShader = compile ps_3_0 fs();
   }
}

#endif
