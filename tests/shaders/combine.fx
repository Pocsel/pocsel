float4x4 mvp : WorldViewProjection;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;

float3 lightPos = float3(0, 0, 0);
float3 lightDiffuse = float3(1.0, 1.0, 1.0);
float3 lightSpecular = float3(0.9, 1.0, 0.8);

sampler2D normals = sampler_state
{
   minFilter = Point;
   magFilter = Point;
};
sampler2D depthBuffer = sampler_state
{
   minFilter = Point;
   magFilter = Point;
};
sampler2D colors = sampler_state
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
    vout.position = mul(mvp, position);
    vout.texCoord = texCoord;
    vout.pos = vout.position;
    return vout;
}

float3 decodePosition(float4 depth, float4 screenPos)
{
    return depth.xyz;
}

float4 fs(in VSout v) : COLOR
{
    float2 coords = v.texCoord * 2;
    //coords.y = 2 - coords.y;
    if (coords.y < 1)
    {
        if (coords.x < 1)
            return tex2D(depthBuffer, coords);
        coords.x = coords.x - 1;
        return tex2D(normals, coords);
    }
    else
    {
        coords.y -= 1;
        if (coords.x < 1)
            return tex2D(colors, coords);
        coords.x = coords.x - 1;

        float3 diffuse = tex2D(colors, coords).rgb;
        float3 vWorldNrm = tex2D(normals, coords).xyz * 2 - 1;
        float3 vWorldPos = decodePosition(tex2D(depthBuffer, coords), v.pos);//tex2D(depthBuffer, coords).xyz;

        float3 vLightDir = normalize(lightPos - vWorldPos);
        float3 vEyeVec = normalize(viewInverse[3].xyz - vWorldPos);
        float3 vDiffuseIntensity = dot(vWorldNrm, vLightDir);
        float3 vSpecularIntensity = pow(max(0, dot(vEyeVec, reflect(-vLightDir, vWorldNrm))), 150);

        float4 color;
        color.rgb = (vDiffuseIntensity * lightDiffuse) * diffuse
             + vSpecularIntensity * lightSpecular.xyz;// * vSpecularMaterial;
        color.a = 1.0;
        return color;
    }
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
       AlphaBlendEnable = false;
       VertexShader = compile vs_2_0 vs();
       PixelShader = compile ps_2_0 fs();
   }
}

#endif