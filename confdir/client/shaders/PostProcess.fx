float4x4 mvp : WorldViewProjection;
float4x4 view : View;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;
float4x4 viewProjectionInverse : ViewProjectionInverse;
float4x4 worldViewProjectionInverse : WorldViewProjectionInverse;
float time = 1.0;

sampler2D normals = sampler_state
{
   magFilter = Point;
   minFilter = Point;
};
sampler2D depthBuffer = sampler_state
{
   magFilter = Point;
   minFilter = Point;
};
sampler2D colors = sampler_state
{
   magFilter = Point;
   minFilter = Point;
};

float3 lightPos = float3(0, 0, 0);
float3 lightDiffuse = float3(1.0, 1.0, 1.0);
float3 lightSpecular = float3(0.9, 1.0, 0.8);

struct VSout
{
   float4 position  : POSITION;
   float2 tex       : TEXCOORD0;
   float4 pos       : TEXCOORD1;
   float3 eyeRay    : TEXCOORD2;
};

float3 CreateEyeRay(float4 p)
{ 
    const float FOV = 90;
    float ViewAspect = 800 / 600; //ScreenDim.x / ScreenDim.y;
    float TanHalfFOV = tan(radians(FOV*0.5));

    float3 ViewSpaceRay = float3(p.x * TanHalfFOV * ViewAspect, p.y * TanHalfFOV, p.w);

    return mul((float3x3)viewInverse, ViewSpaceRay);
}

VSout vs(in float4 position : POSITION, in float2 tex : TEXCOORD0)
{
    VSout vout;
    vout.position = mul(mvp, position);
    vout.pos = vout.position;
    vout.eyeRay = CreateEyeRay(vout.position);
#ifdef DIRECTX
    vout.tex = tex;
#else
    vout.tex = float2(tex.x, 1 - tex.y);
#endif
    return vout;
}

float3 decodeNormals(float4 enc)
{
//    float2 normalInformation = enc.xy;
//    float3 N;
//    N.xy = -normalInformation * normalInformation + normalInformation;
//    N.z = -1;
//    float f = dot(N, float3(1, 1, 0.25));
//    float m = sqrt(f);
//    N.xy = (normalInformation * 8 - 4) * m;
//    N.z = -(1 - 8 * f);
//    return N;
    return enc.xyz * 2.0 - 1.0;
}

float3 decodePosition(float4 pos, float z)
{
    //float2 tcoord = (pos.xy / pos.w).xy * 0.5 + float2(0.5);
    //float3 viewray = float3(pos.xy * (-200 / pos.z ), -200);
    return float3(pos.xy * (-200 / pos.z), -200) * z;
    //float dlight = length(lightPosition - vscoord);
    //float factor = 1.0 - dlight / lradius;
    //if( dlight > lradius ) discard;
    //gl_FragData[0] = vec4( gl_Color.rgb, factor );
}

float4 fs(in VSout i) : COLOR
{
    float3 vDiffuseMaterial = tex2D(colors, i.tex).rgb;
    //float3 vSpecularMaterial = tex2D(SceneMaterialSampler, i.vTex0).a;

    // normals are stored in texture space [0,1] -> convert them back to [-1,+1] range
    float3 vWorldNrm = decodeNormals(tex2D(normals, i.tex));

    float3 vWorldPos = tex2D(depthBuffer, i.tex).xyz;
    //float3 vWorldPos;

    if (i.pos.x < 0)
        vWorldPos = tex2D(depthBuffer, i.tex).xyz;
    else
        //vWorldPos = -i.eyeRay.xyz * tex2D(depthBuffer, i.tex).w;
        vWorldPos = decodePosition(i.pos, -tex2D(depthBuffer, i.tex).w);

    float3 vLightDir = normalize(lightPos - vWorldPos);
    float3 vEyeVec = normalize(viewInverse[3].xyz - vWorldPos);
    float3 vDiffuseIntensity = dot(vWorldNrm, vLightDir);
    float3 vSpecularIntensity = pow(max(0, dot(vEyeVec, reflect(-vLightDir, vWorldNrm))), 150);

    float4 color;
    color.rgb = (vDiffuseIntensity * lightDiffuse) * vDiffuseMaterial
         + vSpecularIntensity * lightSpecular.xyz;// * vSpecularMaterial;
    color.a = 1.0;

    if (i.tex.y < 0.5)
        color.rgb = (vWorldPos);//tex2D(colors, i.tex).rgb;

    // here we add color to show how lighting pass affects the scene
    //color.rgb += i.tex.rgr * 0.5;

    return color;
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
