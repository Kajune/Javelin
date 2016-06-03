cbuffer cbObj : register(b0) {
	matrix ViewProjection;
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
	float lightPower;
	float4 lightPos[MaxLightNum];
	float4 specularLight[MaxLightNum];
	float4 diffuseLight[MaxLightNum];
};

SamplerState smp : register(s0);
Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D ambientMap : register(t2);

struct VS_INPUT {
	float3 position	: POSITION;
	float3 normal	: NORMAL;
	float2 texel	: TEXCOORD;
	matrix World	: MATRIX;
};

struct PS_INPUT {
	float4 position	: SV_POSITION;
	float4 posWorld	: POSITION;
	float4 normal	: NORMAL;
	float2 texel	: TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input, uint id : SV_InstanceID) {
	PS_INPUT output;
	output.posWorld = mul(float4(input.position, 1.0), input.World);
	output.position = mul(output.posWorld, ViewProjection);
	output.normal = normalize(mul(float4(input.normal, 0.0), input.World));
	output.texel = input.texel;
	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET{
	float4 color = float4(0, 0, 0, 0);
	for (uint i = 0; i < numLights; i++) {
		float4 L = lightPos[i] - input.posWorld;
		L = normalize(L) * lightPower / length(L);
		float tmp1 = dot(L, input.normal);
		float tmp2 = dot(reflect(-L, input.normal), float4(eyePos, 1.0) - input.posWorld);
		float tmp3 = pow(tmp2, opacity_reflection_refraction.y);
		color += (diffuse * diffuseMap.Sample(smp, input.texel)
			* tmp1 * diffuseLight[i]);
		+(specular * specularMap.Sample(smp, input.texel)
			* tmp2 * specularLight[i]);
	}
	return ambient * ambientMap.Sample(smp, input.texel) 
		* float4(ambientLight, 1.0f - opacity_reflection_refraction.x)
		+ float4(color.xyz, 1.0f - opacity_reflection_refraction.x);
}
