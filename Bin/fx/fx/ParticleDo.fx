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
	float4 Pos:SV_POSITION;
	float2 tPos:TEXCOORD;
};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float2 tPos:TEXCOORD;
};

VertexOut VS(VertexIn ina)
{
    VertexOut outa;
	outa.Pos  = ina.Pos;
	outa.tPos = ina.tPos;
	return outa;
}
float4 PS( VertexOut outa ) : SV_TARGET
{
	//return ShaderTexture.Sample(SampleType, outa.tPos) ;
	return float4(1.0,0.0,0.0,1.0) ; 
}