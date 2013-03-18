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

float4 fs(in VSout v) : COLOR
{
    if (v.texCoord.y < 1.0/5.0)
    {
        float2 coord = v.texCoord * 5;
        if (coord.x < 1)
            return float4(tex2D(lighting, coord).rgb, 1);
        if (coord.x < 2)
            return float4(tex2D(specular, coord + float2(-1, 0)).rgb, 1);
        if (coord.x < 3)
            return float4(tex2D(diffuse, coord + float2(-2, 0)).rgb, 1);
		if (coord.x < 4)
			return float4(tex2D(normalsDepth, coord + float2(-3, 0)).rgb, 1);
    }

    float4 diff = tex2D(diffuse, v.texCoord);
    float4 spec = tex2D(specular, v.texCoord) * diff.a;
    float3 light = tex2D(lighting, v.texCoord).rgb * diff.a;

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
       VertexShader = compile vs_3_0 vs();
       PixelShader = compile ps_3_0 fs();
   }
}
