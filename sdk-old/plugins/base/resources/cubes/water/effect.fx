float4x4 mvp : WorldViewProjection;
float4x4 mv : WorldView;
float3 lightDirection = {-0.704, -0.704, 0};

float3 fogColor = float3(0.8, 0.8, 0.9);
float fogEnd = 750.0f;

// Water
float time = 0;
float displacement = 4.0;

#ifdef DIRECTX
sampler2D cubeTexture = sampler_state
{
    MinFilter = Linear;
    MagFilter = Point;
    MipFilter = Linear;
};
#else
sampler2D cubeTexture = sampler_state
{
    minFilter = LinearMipMapLinear;
    magFilter = Nearest;
};
#endif

struct VSout
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float lightIntensity : TEXCOORD1;
    float fogFactor : TEXCOORD2;
    // Water:
    //float4 texProj;
};

VSout vs(in float4 position : POSITION, in float3 normal : NORMAL, in float2 texCoord : TEXCOORD0)
{
    VSout vout;

    vout.texCoord = texCoord;//frac(float2(1.0, 2.0) * texCoord);
/*
    float3 normal = float3(1.0, 4.0, 16.0);
    normal = frac(normal * inNormal);
    //Unpack to the -1..1 range
    normal = (normal * 2.0) - 1.0;
*/
    vout.position = mul(mvp, position);
    float fog = clamp(abs(vout.position.z), 0, fogEnd);
    fog = (fogEnd - fog) / fogEnd;
    vout.fogFactor = clamp(fog, 0, 1);
    vout.lightIntensity = dot(-lightDirection, normal) * 0.35 + 0.65;
    return vout;
}

float4 fs(in VSout v) : COLOR
{
    float4 c = tex2D(cubeTexture, v.texCoord);

    c.rgb = v.lightIntensity * lerp(fogColor, c.rgb, v.fogFactor);

    return c;
}

#ifndef DIRECTX

technique tech_glsl
{
    pass p0
    {
        VertexProgram = compile glslv vs();
        FragmentProgram = compile glslf fs();
    }
}
technique tech
{
    pass p0
    {
        VertexProgram = compile arbvp1 vs();
        FragmentProgram = compile arbfp1 fs();
    }
}

#else

technique tech
{
   pass p0
   {
       VertexShader = compile vs_2_0 vs();
       PixelShader = compile ps_2_0 fs();
   }
}

#endif