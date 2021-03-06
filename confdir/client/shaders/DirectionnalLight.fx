float4x4 mvp : WorldViewProjection;
float4x4 view : View;
float4x4 viewProjection : ViewProjection;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;
float4x4 viewProjectionInverse : ViewProjectionInverse;
float4x4 worldView : WorldView;
float4x4 screenWorldViewProjection;

float4   lightAmbientColor = float4(0.2f, 0.2f, 0.2f, 0.2f);
float3   lightDirection = float3(0.0, 0.0, 0.0);
float3   lightDiffuseColor = float3(1.0, 1.0, 1.0);
float3   lightSpecularColor = float3(0.9, 1.0, 0.8);
float4x4 lightViewProjection;

// TODO:
float   materialShininess = 5.0;

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
    float z = 1-enc.z;
    float x = coords.x * 2 - 1;
    float y = coords.y * 2 - 1;
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
    float2 vec[4];// = {float2(1,0),float2(-1,0),float2(0,1),float2(0,-1)};
    vec[0] = float2(1, 0);
    vec[1] = float2(-1, 0);
    vec[2] = float2(0, 1);
    vec[3] = float2(0, -1);

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
    float3 viewLightDirection = normalize(mul((float3x3)view, -lightDirection));

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
    f.diffuse.rgb = lightDiffuseColor;
    f.diffuse.rgb += lightAmbientColor.rgb;
    f.specular.rgb = lightSpecularColor;
    f.diffuse.a = NdL * shadowMap + lightAmbientColor.a;
    f.specular.a = specular * shadowMap * specularPower;
    return f;
}

technique tech
{
   pass p0
   {
        AlphaBlendEnable = true;
        AlphaTestEnable = false;
        AlphaRef = 0;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
        ZEnable = false;
        VertexShader = compile vs_3_0 vs();
        PixelShader = compile ps_3_0 fs();
   }
}
