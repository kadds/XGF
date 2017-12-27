TextureCube gCubeMap;
SamplerState gSampleType;
cbuffer CBMatrix
{
	row_major float4x4 cbWorld;
	row_major float4x4 cbView;
	row_major float4x4 cbProj;
};
cbuffer CBCameraPos
{
	float3 vCameraPos;
};
struct VertexIn
{
	float3 vPos : POSITION;
};
struct VertexOut
{
	float4 vPos : SV_POSITION;
    float3 vTexCoord : TEXCOORD; 
};
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.vPos = mul(float4(vin.vPos,1.0f), cbWorld);
	vout.vPos = mul(float4(vin.vPos,1.0f), cbView);
	vout.vPos = mul(vout.vPos, cbProj).xyww;
	vout.vTexCoord = -vCameraPos + vin.vPos;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gCubeMap.Sample(gSampleType, pin.vTexCoord);
}
