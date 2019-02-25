#ifdef TEXTURE
Texture2D gShaderTexture;
SamplerState gSampleType;
#endif

#ifdef SHADOWMAP_TEXTURE
#define SHADOW_PARAM_WITH_DATA_P ,data.ShadowPos
#define SHADOW_PARAM_P ,ShadowPos
#define SHADOW_PARAM_DECLARATION float4 ShadowPos
#define SHADOW_PARAM_DECLARATION_P ,float4 ShadowPos
#define CALC_SHADOW *calcShadow(ShadowPos)
Texture2D gShadowMapTexture;
#else
#define SHADOW_PARAM_WITH_DATA_P  
#define SHADOW_PARAM_P  
#define SHADOW_PARAM_DECLARATION  
#define SHADOW_PARAM_DECLARATION_P  
#define CALC_SHADOW  ;
#endif

#ifdef HAS_DEF_SAMPLER
SamplerState  gShadowDEFSampleType;
#endif
#ifdef HAS_PCF_SAMPLER
SamplerComparisonState  gShadowPCFSampleType;
#endif

#if defined IS_POINT_LIGHT || defined IS_SPOT_LIGHT || defined IS_DIRECTIONAL_LIGHT
#define HasLight
#endif

struct VertexIn
{
	float3 Pos: POSITION;
#ifdef HasLight
	float3 Normal: NORMAL;
#endif
#ifdef TEXTURE
	float2 Tex: TEXCOORD;
#endif
};
struct VertexOut
{
	float4 Pos: SV_POSITION;
#ifdef HasLight
	float3 Normal: NORMAL;
	float3 worldPos: POSITION0;
#endif
#ifdef TEXTURE
	float2 Tex: TEXCOORD;
#endif
#ifdef SHADOWMAP_TEXTURE
	float4 ShadowPos: POSITION1;
#endif
};
cbuffer CBMatrix:register(b0)
{
	row_major float4x4 cbWrold;
	row_major float4x4 cbView;
	row_major float4x4 cbProj;
#ifdef HasLight
	row_major float4x4 cbNormalMatrix;
#endif
#ifdef SHADOWMAP_TEXTURE
	row_major float4x4 cbShadowViewProj;
#endif
};

cbuffer CBObject:register(b0)
{
	float3 cbColor;
	float3 cbEmissive; // 自发光
	float3 cbDiffuse; // 漫反射
};
struct DirectionalLight
{
	float3 color;
	float3 direction;
};
struct PointLight
{
	float3 color;
	float __y;
	float3 position;
	// 占位
	float __x;
	// 光衰
	float constantValue;
	float linearValue;
	float quadraticValue;
};
struct SpotLight
{
	float3 color;
	float __a;
	float3 position;
	float __b;
	float3 direction;
	// 外张角
	float cutoffOuter;
	// 张角（弧度制）
	float cutoff;
	// 光衰
	float constantValue;
	float linearValue;
	float quadraticValue;
};


cbuffer CBExtra:register(b1)
{
#ifdef IS_BASE
	float3 cbAmbient;
	float __xx;
#endif

#if defined IS_POINT_LIGHT
	PointLight pointLights;
#elif defined IS_DIRECTIONAL_LIGHT
	DirectionalLight directionalLights;
#elif defined IS_SPOT_LIGHT
	SpotLight spotLights;
#endif
};
#ifdef SHADOWMAP_TEXTURE
float calcShadow(SHADOW_PARAM_DECLARATION)
{
	float3 shadowPos = ShadowPos.xyz / ShadowPos.w;
	float2 shadowTex;
	shadowTex.x = shadowPos.x * 0.5f + 0.5f;
	shadowTex.y = shadowPos.y * -0.5f + 0.5f;
	// shadowPos.z = shadowPos.z * 0.5f + 0.5f;
	if (saturate(shadowTex.x) == shadowTex.x && saturate(shadowTex.y) == shadowTex.y && saturate(shadowPos.z) == shadowPos.z)
	{
#ifdef HAS_PCF_SAMPLER
		float shadow = gShadowMapTexture.SampleCmpLevelZero(gShadowPCFSampleType, shadowTex, shadowPos.z).r;
#endif
#ifdef HAS_DEF_SAMPLER
		float shadow = saturate(gShadowMapTexture.Sample(gShadowDEFSampleType, shadowTex).r - shadowPos.z);
#endif
		return shadow;
	}
	return 1.0;
}
#endif
float calcDiffuse(float3 normal, float3 lightDirection)
{
	float diff = max(dot(normal, lightDirection), 0.0);
	return diff;
}

float3 calc(float3 color, float3 normal, float3 lightDirection, float fact 
SHADOW_PARAM_DECLARATION_P
)
{
	float diff = calcDiffuse(normal, lightDirection);
	float3 diffuse = diff * color * cbDiffuse;
	return diffuse * fact CALC_SHADOW;
}
float3 calcPointLight(PointLight light, float3 normal, float3 pos 
SHADOW_PARAM_DECLARATION_P
)
{
	float3 lightDirection = normalize(light.position - pos);
	float distance = length(light.position - pos);
	float atte = 1.0f / (light.constantValue + light.linearValue * distance + light.quadraticValue * distance * distance);
	atte = saturate(atte);
	return calc(light.color, normal, lightDirection, atte SHADOW_PARAM_P);
}
float3 calcDirectionalLight(DirectionalLight light, float3 normal 
SHADOW_PARAM_DECLARATION_P
)
{
	float3 lightDirection = normalize(-light.direction);
	return calc(light.color, normal, lightDirection, 1.0f SHADOW_PARAM_P);
}
float3 calcSpotLight(SpotLight light, float3 normal, float3 pos
SHADOW_PARAM_DECLARATION_P
)
{
	float3 lightDirection = normalize(light.position - pos);
	float dist = length(light.position - pos);
	float atte = 1.0f / (light.constantValue + light.linearValue * dist + light.quadraticValue * dist * dist);
	float epsilon = light.cutoff - light.cutoffOuter;
	float theta = dot(lightDirection, normalize(-light.direction));
	float intensity = saturate((theta - light.cutoffOuter) / epsilon); 
	atte = saturate(atte);
	return calc(light.color, normal, lightDirection, intensity * atte SHADOW_PARAM_P);
}
float4 PS(VertexOut data) : SV_TARGET
{
#ifdef HasLight
	float3 norm = normalize(data.Normal);
#endif
#ifdef IS_BASE
	float3 r = cbAmbient;
#else
	float3 r = float3(0, 0, 0);
#endif
#if defined IS_POINT_LIGHT
	r += calcPointLight(pointLights, norm, data.worldPos SHADOW_PARAM_WITH_DATA_P);
#elif defined IS_DIRECTIONAL_LIGHT
	r += calcDirectionalLight(directionalLights, norm SHADOW_PARAM_WITH_DATA_P);
#elif defined IS_SPOT_LIGHT
	r += calcSpotLight(spotLights, norm, data.worldPos SHADOW_PARAM_WITH_DATA_P);
#endif

#ifdef TEXTURE
	r *= gShaderTexture.Sample(gSampleType, data.Tex);
#endif
	return float4(r * cbColor, 1.0);
}
VertexOut VS(VertexIn data)
{
	VertexOut vout;
	vout.Pos= mul(float4(data.Pos, 1.0f), cbWrold);
#ifdef HasLight
	vout.worldPos = vout.Pos.xyz;
#endif
	vout.Pos = mul(vout.Pos, cbView);
	vout.Pos = mul(vout.Pos, cbProj);
#ifdef HasLight
	vout.Normal = mul( float4(data.Normal, 1.0), cbNormalMatrix).xyz;
#endif

#ifdef SHADOWMAP_TEXTURE
	vout.ShadowPos = mul(mul(float4(data.Pos, 1.0f), cbWrold), cbShadowViewProj);
#endif
#ifdef TEXTURE
	vout.Tex = data.Tex;
#endif
	return vout;
}