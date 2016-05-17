cbuffer cbObj : register(b0) {
	matrix World;
	matrix ViewProjection;
};

cbuffer cbMat : register(b1) {
	float4 diffuse;
};

SamplerState smp : register(s0);
Texture2D diffuseMap : register(t0);

struct VS_INPUT {
	float3 position	: POSITION;
	float3 normal	: NORMAL;
	float2 texel	: TEXCOORD;
};

struct PS_INPUT {
	float4 position	: SV_POSITION;
	float3 normal	: NORMAL;
	float2 texel	: TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input) {
	PS_INPUT output;
	output.position = mul(mul(float4(input.position, 1.0), World), ViewProjection);
	output.normal = input.normal;
	output.texel = input.texel;
	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET{
	return diffuse * diffuseMap.Sample(smp, input.texel);
}
