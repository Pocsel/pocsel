float4x4 mvp : WorldViewProjection;
float4x4 viewInverse : ViewInverse;
float4x4 projectionInverse : ProjectionInverse;
float4x4 viewProjectionInverse : ViewProjectionInverse;
float4x4 quadWorldViewProjection;

float3 lightPos = float3(0, 0, 0);
float3 lightDiffuse = float3(1.0, 1.0, 1.0);
float3 lightSpecular = float3(0.9, 1.0, 0.8);

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
sampler2D normalsDepth = sampler_state
{
    minFilter = Point;
    magFilter = Point;
};

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
    float y = (1 - coords.y) * 2 - 1;
    float4 projPos = float4(x, y, z, 1.0);
    float4 pos = mul(projectionInverse, projPos);
    return pos.xyz / pos.w;
}

float4 fs(in VSout v) : COLOR
{
    if (v.texCoord.y > 4.0/5.0)
    {
        float2 coord = (v.texCoord * 5) - float2(0, 4);
        if (coord.x < 1)
            return float4(tex2D(lighting, float2(coord.x, 1 - coord.y)).rgb, 1);
        if (coord.x < 2)
            return float4(tex2D(specular, float2(coord.x, 1 - coord.y) + float2(-1, 0)).rgb, 1);
        if (coord.x < 3)
            return float4(tex2D(diffuse, coord + float2(-2, 0)).rgb, 1);
        if (coord.x < 4)
        {
            float3 normal = decodeNormals(tex2D(normalsDepth, coord + float2(-3, 0)));
            return float4(normal * 0.5 + 0.5, 1);
        }
        if (coord.x < 5)
        {
            float2 screen = coord + float2(-4, 0);
            float3 pos = decodePosition(tex2D(normalsDepth, coord + float2(-4, 0)), screen);
            return float4(pos, 1);
        }
    }

    float4 diff = tex2D(diffuse, v.texCoord);
    float4 spec = tex2D(specular, float2(v.texCoord.x, 1 - v.texCoord.y)) * diff.a;
    float3 light = tex2D(lighting, float2(v.texCoord.x, 1 - v.texCoord.y)).rgb * diff.a;

    float3 color = light * diff.rgb + diff.rgb * (1 - diff.a);
    color += spec.rgb;

    if (light.r < 0.1 && diff.r == 0)
        color = float3(0.9, 0.9, 0.3);

    return 0.8 * float4(color, diff.a) + 0.2 * diff;
}

technique tech
{
   pass p0
   {
       AlphaBlendEnable = false;
       ZEnable = false;
       VertexShader = compile vs_3_0 vs();
       PixelShader = compile ps_3_0 fs();
   }
}
