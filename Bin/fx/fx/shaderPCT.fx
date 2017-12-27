Texture2D gShaderTexture;
SamplerState gSampleType;
cbuffer CBMatrix
{
	row_major float4x4 cbWorld;
	row_major float4x4 cbView;
	row_major float4x4 cbProj;
};
struct VertexIn
{
	float3 vPos:POSITION;
	float4 vColor : COLOR;
	float2 vTex:TEXCOORD;
};

struct VertexOut
{
	float4 vPos:SV_POSITION;
	float4 vColor : COLOR;
	float2 vTex:TEXCOORD;
};

VertexOut VS(VertexIn ina)
{
    VertexOut outa;
	outa.vPos = mul(float4(ina.vPos,1.0f), cbWorld);
	outa.vPos = mul(outa.vPos, cbView);
	outa.vPos = mul(outa.vPos, cbProj);
	outa.vColor = ina.vColor;
	outa.vTex = ina.vTex;
	return outa;
}
float4 PS( VertexOut outa ) : SV_TARGET
{
	float4 color;
    color = gShaderTexture.Sample(gSampleType, outa.vTex);
	return color * outa.vColor;
}
