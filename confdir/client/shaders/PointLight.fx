float4x4 mvp : WorldViewProjection;
float4x4 view : View;
float4x4 viewProjection : ViewProjection;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;
float4x4 viewProjectionInverse : ViewProjectionInverse;
float4x4 worldView : WorldView;
float4x4 screenWorldViewProjection;

float3  lightAmbientColor = float3(0.2f, 0.2f, 0.2f);
float3  lightPosition = float3(0.0, 0.0, 0.0);
float3  lightRange = float3(0.0, 0.0, 0.0);
float3  lightDiffuseColor = float3(1.0, 1.0, 1.0);
float3  lightSpecularColor = float3(0.9, 1.0, 0.8);

// TODO:
float   materialShininess = 5.0;

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
    float3 viewLightDirection = normalize(mul((float3x3)worldView, -lightPosition));

    float4 encNormalDepth = tex2D(normalDepth, v.texCoord);
    float3 viewNormal = decodeNormals(encNormalDepth);// * 2 - 1;
    float3 viewPosition = decodePosition(encNormalDepth, v.texCoord);

    float NdL = max(0, dot(viewNormal, viewLightDirection));

    float3 viewDirection = normalize(viewPosition);
    float3 reflection = reflect(viewLightDirection, viewNormal);
    float specular = pow(max(0.0, dot(reflection, viewDirection)), materialShininess);

    FSout f;
    f.diffuse.rgb = NdL * lightDiffuseColor;
    f.diffuse.rgb += lightAmbientColor;
    f.specular.rgb = specular * lightSpecularColor;
    f.diffuse.a = NdL;
    f.specular.a = specular;
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
        AlphaBlendEnable = True;
        AlphaTestEnable = True;
        AlphaRef = 0;
        SrcBlend = One;
        DestBlend = One;
        ZEnable = false;
        VertexShader = compile vs_3_0 vs();
        PixelShader = compile ps_3_0 fs();
   }
}

#endif