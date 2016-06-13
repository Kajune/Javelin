cbuffer cbObj : register(b0) {
	matrix World;
	matrix ViewProjection;
	matrix ViewProjectionShadow;
};

cbuffer cbMat : register(b1) {
	float4 diffuse;
	float4 specular;
	float4 ambient;
	float4 opacity_reflection_refraction;
};

static const uint MaxLightNum = 16;

cbuffer cbLight : register(b2) {
	float3 eyePos;
	uint numLights;
	float3 ambientLight;
	float dummy;
	float lightPower[MaxLightNum];
	float4 lightPos[MaxLightNum];
	float4 specularLight[MaxLightNum];
	float4 diffuseLight[MaxLightNum];
};

SamplerState smp : register(s0);
Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D ambientMap : register(t2);

Texture2D shadowMap : register(t7);

struct VS_INPUT {
	float3 position	: POSITION;
	float3 normal	: NORMAL;
	float2 texel	: TEXCOORD;
};

struct PS_INPUT {
	float4 position	: SV_POSITION;
	float4 posWorld	: POSITION;
	float4 normal	: NORMAL;
	float2 texel	: TEXCOORD0;
	float4 posShadow	: POSITION_SM;
};

PS_INPUT VS(VS_INPUT input) {
	PS_INPUT output;
	output.posWorld = mul(float4(input.position, 1.0), World);
	output.position = mul(output.posWorld, ViewProjection);
	output.normal = normalize(mul(float4(input.normal, 0.0), World));
	output.texel = input.texel;
	output.posShadow = mul(output.posWorld, ViewProjectionShadow);
	return output;
}

static const float depthBias = 0.00001;

float4 PS(PS_INPUT input) : SV_TARGET{
	float4 color = float4(0, 0, 0, 0);
	for (uint i = 0; i < numLights; i++) {
		float4 L = lightPos[i] - input.posWorld;
		L = normalize(L) * lightPower[i] / length(L);
		float tmp1 = dot(L, input.normal);
		float tmp2 = dot(reflect(-L, input.normal), float4(eyePos, 1.0) - input.posWorld);
		float tmp3 = pow(tmp2, opacity_reflection_refraction.y);
		color += (diffuse * diffuseMap.Sample(smp, input.texel)
			* tmp1 * diffuseLight[i]);
		+(specular * specularMap.Sample(smp, input.texel)
			* tmp2 * specularLight[i]);
	}
	float3 posSM = input.posShadow.xyz / input.posShadow.w;
	return ambient * ambientMap.Sample(smp, input.texel) * float4(ambientLight, 1.0)
		+ float4(color.xyz, 1.0f - opacity_reflection_refraction.x)
		* (shadowMap.Sample(smp,
			float2((1.0 + posSM.x) / 2, (1.0 - posSM.y) / 2)).x + depthBias > posSM.z ? 1.0 : 0.0);
}

float4 VS_Shadow(VS_INPUT input) : SV_POSITION{
	float4 pos4 = mul(mul(float4(input.position, 1.0), World), ViewProjectionShadow);
	return pos4 / pos4.w;
}