cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
};
cbuffer CBMatrixView:register(b1)
{
	matrix ViewInves;
};
struct VS_OUTPUT  
{  
    float4 ObjectPosition :POSITION0; 
	float4 Data1 :POSITION1;
	float4 Color:COLOR;
};
struct ParticleData
{
    float3 position;
    float4 velocity;
	float4 Color:COLOR;
};
struct VertexIn
{
	float3 position:POSITION0;
	float4 velocity:POSITION1;
	float4 Color:COLOR;
    uint id : SV_VERTEXID;
};
struct GS_OUTPUT  
{  
    float4 Position :SV_POSITION; 
	float4 Color:COLOR;	
	float2 tps:TEXCOORD;	
};
Texture2D ShaderTexture:register(t0);  //纹理资源
SamplerState SampleType:register(s0);   //采样方式
StructuredBuffer<ParticleData>   g_bufPosVelo;
[maxvertexcount(4)]  
void GS(point VS_OUTPUT vin[1], inout TriangleStream<GS_OUTPUT> triStream)  
{
	float3 v[4];
	float w = vin[0].Data1.x / 2;
	float h = vin[0].Data1.y / 2;
	
	v[0] = float3(vin[0].ObjectPosition.x - w, vin[0].ObjectPosition.y - h, 0);
	v[1] = float3(vin[0].ObjectPosition.x + w, vin[0].ObjectPosition.y - h, 0);
	v[2] = float3(vin[0].ObjectPosition.x - w, vin[0].ObjectPosition.y + h, 0);
	v[3] = float3(vin[0].ObjectPosition.x + w, vin[0].ObjectPosition.y + h, 0);
	GS_OUTPUT op;
	float2 k[4];
	k[0] = float2(0,0);
	k[1] = float2(0,1);
	k[3] = float2(1,1);
	k[2] = float2(1,0);
	op.Color = vin[0].Color;
	for(int i = 0; i < 4; i++)
	{
		op.Position = mul( mul(mul(mul( float4(v[i], 1.0),ViewInves) ,World),View), Proj);
		op.tps = k[i];
		triStream.Append(op);
	}
	triStream.RestartStrip();
}
VS_OUTPUT VS(VertexIn vin)
{
	VS_OUTPUT vout;
	vout.ObjectPosition = float4( vin.position,0);
	vout.Data1 = vin.velocity;
	vout.Color = vin.Color;
	//vout.Data1 = vin.Data1;
	return vout;
}
float4 PS( GS_OUTPUT outa ) : SV_TARGET
{
	float4 t = ShaderTexture.Sample(SampleType, outa.tps);	
	
	return t * outa.Color;
}