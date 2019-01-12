#ifdef TEXTURE
Texture2D gShaderTexture;
SamplerState gSampleType;
#endif
struct VertexIn
{
	float3 Pos: POSITION;
	float3 Normal: NORMAL;
#ifdef TEXTURE
	float2 Tex: TEXCOORD;
#endif
};
struct VertexOut
{
	float4 Pos: SV_POSITION;
	float3 worldPos: POSITION;
	float3 Normal: NORMAL;
#ifdef TEXTURE
	float2 Tex: TEXCOORD;
#endif
};
cbuffer CBMatrix:register(b0)
{
	row_major float4x4 cbWrold;
	row_major float4x4 cbView;
	row_major float4x4 cbProj;
};

cbuffer CBObject:register(b0)
{
	float3 cbColor;
	float3 cbEmissive; // 自发光
	float3 cbDiffuse; // 漫反射
	float3 cbSpecular; // 高光
	float cbShininess; // 高光指数
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
#if N_OF_DIRECTIONALLIGHT != 0 || N_OF_POINTLIGHT != 0 || N_OF_SPOTLIGHT != 0
#define HasLight
#endif

#ifdef HasLight
cbuffer CBViewPos:register(b1)
{
	float3 viewPos;
}
cbuffer CBExtra:register(b2)
#else
cbuffer CBExtra:register(b1)
#endif
{
	float3 cbAmbient; // 环境光之和
	float __none;
#if N_OF_DIRECTIONALLIGHT!=0
	DirectionalLight directionalLights[N_OF_DIRECTIONALLIGHT];
#endif
#if N_OF_POINTLIGHT!=0
	PointLight pointLights[N_OF_POINTLIGHT];
#endif
#if N_OF_SPOTLIGHT != 0
	SpotLight spotLights[N_OF_SPOTLIGHT];
#endif
};

float SpotLighting(float3 lightDir, float3 plightDir, float2 cosCone)
{
	float cosDirection = dot(lightDir, plightDir);
	return smoothstep(cosCone.x, cosCone.y, cosDirection);
}
float calcDiffuse(float3 normal, float3 lightDirection)
{
	float diff = max(dot(normal, lightDirection), 0.0);
	return diff;
}
float calcSpecular(float3 normal, float3 lightDirection, float3 viewDirection)
{
	float spec = 0;
#ifdef LIGHT_PHONG
	float3 refVec = reflect(-lightDirection, normal);
	spec = pow(max(dot(refVec, viewDirection), 0), cbShininess);
#else
	float3 halfVec = normalize(lightDirection + viewDirection);
	spec = pow(max(dot(normal, halfVec), 0), cbShininess);
#endif
	return spec;
}
float3 calc(float3 color, float3 normal, float3 viewDirection, float3 lightDirection)
{
	float diff = calcDiffuse(normal, lightDirection);
	float3 diffuse = diff * color * cbDiffuse;
	float spec = 0;
	if(diff > 0)
	{
		spec = calcSpecular(normal, lightDirection, viewDirection);
	}
	float3 specular = spec * color * cbSpecular;
	
	return diffuse + specular;
}
float3 calcPointLight(PointLight light, float3 normal, float3 viewDirection, float3 pos)
{
	float3 lightDirection = normalize(light.position - pos);
	float distance = length(light.position - pos);
	float atte = 1.0f / (light.constantValue + light.linearValue * distance + light.quadraticValue * distance * distance);
	atte = saturate(atte);
	return calc(light.color, normal, viewDirection, lightDirection);
}
float3 calcDirectionalLight(DirectionalLight light, float3 normal, float3 viewDirection)
{
	float3 lightDirection = normalize(-light.direction);
	return calc(light.color, normal, viewDirection, lightDirection);
}
float3 calcSpotLight(SpotLight light, float3 normal, float3 viewDirection, float3 pos)
{
	float3 lightDirection = normalize(light.position - pos);
	float distance = length(light.position - pos);
	float atte = 1.0f / (light.constantValue + light.linearValue * distance + light.quadraticValue * distance * distance);
	float epsilon = light.cutoff - light.cutoffOuter;
	float theta = dot(lightDirection, normalize(-light.direction));
	float intensity = saturate((theta - light.cutoffOuter) / epsilon); 
	
	float diff = calcDiffuse(normal, lightDirection);
	float3 diffuse = diff * light.color * cbDiffuse;
	float spec = 0;
	if(diff > 0)
	{
		spec = calcSpecular(normal, lightDirection, viewDirection);
	}
	float3 specular = spec * light.color * cbSpecular;
	atte = saturate(atte);
	return (diffuse + specular) * intensity * atte;
}
float4 PS(VertexOut data) : SV_TARGET
{
	float3 norm = normalize(data.Normal);
#ifdef HasLight
	float3 viewDirection = normalize(viewPos - data.worldPos);
#endif
	float3 r = float3(0,0,0);
#if N_OF_DIRECTIONALLIGHT!=0
	for(int i = 0; i < N_OF_DIRECTIONALLIGHT; i++)
	{
		r += calcDirectionalLight(directionalLights[i], norm, viewDirection);
	}
#endif
#if N_OF_POINTLIGHT!=0
	for(int i = 0; i < N_OF_POINTLIGHT; i++)
	{
		r += calcPointLight(pointLights[i], norm, viewDirection, data.worldPos);
	}
#endif
#if N_OF_SPOTLIGHT!=0
	for(int i = 0; i < N_OF_SPOTLIGHT; i++)
	{
		r += calcSpotLight(spotLights[i], norm, viewDirection, data.worldPos);
	}
#endif
	r += cbAmbient;
	float4 outColor = float4(r * cbColor, 1.0);
#ifdef TEXTURE
	outColor *= gShaderTexture.Sample(gSampleType, data.Tex);
#endif
	return outColor + float4(cbEmissive, 0.0);
}
VertexOut VS(VertexIn data)
{
	VertexOut vout;
	vout.Pos= mul(float4(data.Pos, 1.0f), cbWrold);
	vout.worldPos = vout.Pos.xyz;
	vout.Pos = mul(vout.Pos, cbView);
	vout.Pos = mul(vout.Pos, cbProj);
	//vout.Normal = data.Normal;
	//vout.Normal = mat3(transpose(inverse(cbWrold))) * data.Normal;
	vout.Normal = mul( float4(data.Normal, 1.0), cbWrold).xyz;
#ifdef TEXTURE
	vout.Tex = data.Tex;
#endif
	return vout;
}