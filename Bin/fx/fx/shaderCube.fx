TextureCube gCubeMap:register(t0);  //纹理资源
SamplerState SampleType:register(s0);   //采样方式
cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
};
struct VertexIn
{
	float3 Pos : POSITION;
};
struct VertexOut
{
	float4 Pos : SV_POSITION;
    float3 texCoord : TEXCOORD; 
};
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.texCoord = vin.Pos;
	vout.Pos = mul(float4(vin.Pos,1.0f), World);
	vout.Pos = mul(vout.Pos, View);
	vout.Pos = mul(vout.Pos, Proj);
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gCubeMap.Sample(SampleType, pin.texCoord);
}
