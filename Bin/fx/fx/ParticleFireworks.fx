Texture2D gShaderTexture;
SamplerState gSampleType;
cbuffer CBMatrix
{
	row_major float4x4 cbWorld;
	row_major float4x4 cbView;
	row_major float4x4 cbProj;
};
cbuffer CBCameraPos
{
	row_major float4x4 vCameraPos;
};
struct VS_OUTPUT  
{  
    float3 vPos :POSITION0; 
	float4 vData1 :POSITION1;
	float4 vColor:COLOR;
};
struct ParticleData
{
    float3 vPos;
    float4 vVelocity;
	float4 vColor:COLOR;
};
struct VertexIn
{
	float3 vPos:POSITION0;
	float4 vVelocity:POSITION1;
	float4 vColor:COLOR;
    uint id : SV_VERTEXID;
};
struct GS_OUTPUT  
{  
    float4 vPos :SV_POSITION; 
	float4 vColor:COLOR;	
	float2 vTex:TEXCOORD;	
};

[maxvertexcount(4)]  
void GS(point VS_OUTPUT vin[1], inout TriangleStream<GS_OUTPUT> triStream)  
{
	float3 v[4];
	float w = vin[0].vData1.x / 2;
	float h = vin[0].vData1.y / 2;
	
	v[0] = float3(- w, - h, 0);
	v[1] = float3(+ w, - h, 0);
	v[2] = float3(- w, + h, 0);
	v[3] = float3(+ w, + h, 0);
	GS_OUTPUT op;
	float2 k[4];
	k[0] = float2(0,0);
	k[1] = float2(0,1);
	k[3] = float2(1,1);
	k[2] = float2(1,0);
	op.vColor = vin[0].vColor;
	for(int i = 0; i < 4; i++)
	{
		op.vPos = mul( mul(mul( float4(mul(v[i], (float3x3)vCameraPos) + vin[0].vPos, 1.0) ,cbWorld),cbView), cbProj);
		op.vTex = k[i];
		triStream.Append(op);
	}
	triStream.RestartStrip();
}
VS_OUTPUT VS(VertexIn vin)
{
	VS_OUTPUT vout;
	vout.vPos = vin.vPos;
	vout.vData1 = vin.vVelocity;
	vout.vColor = vin.vColor;
	return vout;
}
float4 PS( GS_OUTPUT outa ) : SV_TARGET
{
	float4 t = gShaderTexture.Sample(gSampleType, outa.vTex);
	return t * outa.vColor;
}