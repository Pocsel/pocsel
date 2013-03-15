float4x4 mvp : WorldViewProjection;

sampler2D baseTex = sampler_state
{
   minFilter = Point;
   magFilter = Point;
};

float4 clearColor = float4(0, 0, 0, 1);
float4 clearNormal = float4(0, 1, 0, 0);
float4 clearPosition = float4(0, 0, 0, 0);

struct VSout
{
   float4 position : POSITION;
};

struct FSout
{
    float4 color : COLOR0;
    float4 normal : COLOR1;
    float4 position : COLOR2;
};

VSout vs(in float4 position : POSITION)
{
    VSout vout;
    vout.position = mul(mvp, position);
    return vout;
}

FSout fs(in VSout v)
{
    FSout f;
    f.color = clearColor;
    f.normal = clearNormal;
    f.position = clearPosition;
    return f;
}

technique tech
{
   pass p0
   {
       VertexShader = compile vs_2_0 vs();
       PixelShader = compile ps_2_0 fs();
   }
}
