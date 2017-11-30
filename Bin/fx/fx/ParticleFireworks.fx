cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
};
struct VS_OUTPUT  
{  
    float4 ObjectPosition :POSITION0; 
	float4 Data1 :POSITION1;
	float4 Data2 :POSITION2;
};
struct VertexIn
{
	float3 Pos :POSITION0;
	float4 Data1 :POSITION1;
	float4 Data2 :POSITION2;
};
struct GS_OUTPUT  
{  
    float4 Position :SV_POSITION;  
	float2 tps:TEXCOORD;	
};  
[maxvertexcount(100)]  
void GS(point VS_OUTPUT vin[1], inout TriangleStream<GS_OUTPUT> triStream)  
{
	float4 v[4];
	float w = vin[0].Data1.x / 2;
	float h = vin[0].Data1.y / 2;
	float timeall = vin[0].Data1.w;
	float timepass = vin[0].Data1.z;
	float startRadius =0 ;
	float endRadius = 0;
	v[0] = float4(vin[0].ObjectPosition.x - w, vin[0].ObjectPosition.y - h, 0.0f, 1.0f);
	v[1] = float4(vin[0].ObjectPosition.x + w, vin[0].ObjectPosition.y - h, 0.0f,1.0f);
	v[2] = float4(vin[0].ObjectPosition.x + w, vin[0].ObjectPosition.y + h , 0.0f, 1.0f);
	v[3] = float4(vin[0].ObjectPosition.x - w,vin[0].ObjectPosition.y + h, 0.0f, 1.0f);
    GS_OUTPUT op;
	for(int i = 0; i < 4; ++i)
	{
		op.Position =  mul( mul(mul(v[i],World),View), Proj);
		op.tps = float2(0.2,0.2);
		triStream.Append(op);
	}
	op.Position = mul( mul(mul(v[0],World),View), Proj);
	op.tps = float2(0.2,0.2);
	triStream.Append(op);
	triStream.RestartStrip();
}
VS_OUTPUT VS(VertexIn vin)
{
	VS_OUTPUT vout;
	vout.ObjectPosition = float4(vin.Pos,1.0f);
	vout.Data1 = vin.Data1;
	vout.Data2 = vin.Data2;
	return vout;
}

float4 PS(GS_OUTPUT pin) : SV_Target
{
	return float4(1.0,0.0,0.0,1.0);
}