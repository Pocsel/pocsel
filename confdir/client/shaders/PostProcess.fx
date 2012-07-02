float4x4 mvp : WorldViewProjection;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;
float4x4 viewProjectionInverse : ViewProjectionInverse;
float4x4 quadWorldViewProjection;

float3 lightPos = float3(0, 0, 0);
float3 lightDiffuse = float3(1.0, 1.0, 1.0);
float3 lightSpecular = float3(0.9, 1.0, 0.8);

#ifdef DIRECTX
texture lighting;
texture specular;
texture diffuse;

sampler sLighting = sampler_state
{
    Texture = <lighting>;
    minFilter = Point;
    magFilter = Point;
};
sampler sSpecular = sampler_state
{
    Texture = <specular>;
    minFilter = Point;
    magFilter = Point;
};
sampler sDiffuse = sampler_state
{
    Texture = <diffuse>;
    minFilter = Point;
    magFilter = Point;
};

#define lighting sLighting
#define specular sSpecular
#define diffuse sDiffuse

#else
sampler2D lighting = sampler_state
{
    minFilter = Point;
    magFilter = Point;
};
sampler2D specular = sampler_state
{
    minFilter = Point;
    magFilter = Point;
};
sampler2D diffuse = sampler_state
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

VSout vs(in float4 position : POSITION, in float2 texCoord : TEXCOORD0)
{
    VSout vout;
    vout.position = mul(quadWorldViewProjection, position);
    vout.texCoord = texCoord;
    return vout;
}

float4 fs(in VSout v) : COLOR
{
    //if (v.texCoord.y < 0.25)
    //{
    //    float2 coord = v.texCoord * 4;
    //    if (coord.x < 1)
    //        return float4(tex2D(lighting, coord).rgb, 1);
    //    if (coord.x < 2)
    //        return float4(tex2D(specular, coord + float2(-1, 0)).rgb, 1);
    //    if (coord.x < 3)
    //        return float4(tex2D(diffuse, coord + float2(-2, 0)).rgb, 1);
    //}
#ifdef DIRECTX
    float4 diff = tex2D(diffuse, v.texCoord);
    float4 spec = tex2D(specular, v.texCoord);
    float3 light = tex2D(lighting, v.texCoord).rgb;
#else
    v.texCoord = float2(v.texCoord.x, 1-v.texCoord.y);
    float4 diff = tex2D(diffuse, v.texCoord);
    float4 spec = tex2D(specular, v.texCoord);
    float3 light = tex2D(lighting, v.texCoord).rgb;
#endif

    float3 color = light * diff.rgb;
    color += spec.rgb;

    if (light.r < 0.1 && diff.r == 0)
        color = float3(0.9, 0.9, 0.3);

    return float4(color, diff.a);
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
       AlphaBlendEnable = false;
       VertexShader = compile vs_3_0 vs();
       PixelShader = compile ps_3_0 fs();
   }
}

#endif
