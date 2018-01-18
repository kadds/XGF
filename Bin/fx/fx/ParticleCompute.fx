struct ParticleData
{
    float3 position;
    float4 velocity;
	float4 color:COLOR;
};

StructuredBuffer<ParticleData>  inputConstantParticleData;
RWStructuredBuffer<ParticleData> outputAppParticleData;
Texture1D gRandomTex;
SamplerState SampleType:register(s0);   //采样方式
float3 GetRandomVec3(float offset)
{
	float u= offset;
	float3 v=gRandomTex.SampleLevel(SampleType,u,0);
	return v;
}
[numthreads(32, 1, 1)]
void CS(uint3 DTid : SV_DispatchThreadID )
{
	ParticleData p;
	p = inputConstantParticleData[DTid.x];
	//p.color = float4(GetRandomVec3(DTid.x),1);
	p.position = GetRandomVec3(DTid.x);
	p.velocity = float4(10,10,2,2);
	outputAppParticleData[DTid.x] = p;
}