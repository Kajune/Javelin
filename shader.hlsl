cbuffer cbObj : register(b0) {
	matrix ViewProjection;
};

struct VS_INPUT {
	float3 position	: POSITION;
};

struct PS_INPUT {
	float4 position	: SV_POSITION;
};

PS_INPUT VS(VS_INPUT input) {
	PS_INPUT output;
	output.position = mul(float4(input.position, 1.0), ViewProjection);
	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET{
	return float4(0.5, 0.5, 0.5, 1);
}

