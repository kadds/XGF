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
};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float4 color : COLOR;
};

VertexOut VS(VertexIn ina)
{
    VertexOut outa;
	outa.Pos = mul(float4(ina.Pos,1.0f), World);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);
	outa.color = ina.color;
	return outa;
}
float4 PS( VertexOut outa ) : SV_TARGET
{
	//return float4(0.0f,0.0f,0.0f,1.0f) ; 
	return outa.color ; 
}
