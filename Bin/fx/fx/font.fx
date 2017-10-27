Texture2D ShaderTexture:register(t0);  //纹理资源
SamplerState SampleType:register(s0);   //采样方式

cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
};

struct VertexIn
{
	float3 Pos:POSITION;
	float4 color : COLOR;
	float2 Tex:TEXCOORD0;
};
struct VertexOut
{
	float4 Pos:SV_POSITION;
	float4 color : COLOR;  
	float2 Tex:TEXCOORD0;
};
float4 PS( VertexOut outa ) : SV_TARGET
{
    float4 color;
    color = ShaderTexture.Sample(SampleType, outa.Tex);
	color.rgb = 1;
	//return float4(1.0,1.0,1.0,1.0);
	return color * outa.color;
}

VertexOut VS(VertexIn ina)
{
    VertexOut outa;
	outa.Pos = mul(float4(ina.Pos,1.0f), World);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	outa.Tex = ina.Tex;
	outa.color = ina.color;
	//outa.Pos = float4(ina.Pos,1.0f);
	return outa;
}