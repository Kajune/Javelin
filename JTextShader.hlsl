cbuffer cbVS : register(b0) {
	matrix TexMatrix;
};

cbuffer cbPS : register(b1) {
	float4 Color;
};

Texture2D tex : register(t0);
SamplerState smp : register(s0);

static const float2 vertexPos[6] = {
	{ -1, -1 },
	{ -1, 1 },
	{ 1, -1 },
	{ -1, 1 },
	{ 1, 1 },
	{ 1, -1 },
};

static const float2 texPos[6] = {
	{0, 1},
	{0, 0},
	{1, 1},
	{0, 0},
	{1, 0},
	{1, 1},
};

struct PS_INPUT {
	float4 Pos	: SV_POSITION;
	float2 Tex	: TEXCOORD0;
};

PS_INPUT VS(uint id : SV_VertexID) {
	PS_INPUT output;
	output.Pos = mul(float4(vertexPos[id], 0.0, 1.0), TexMatrix);
	output.Tex = texPos[id];
	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET{
	return Color * tex.Sample(smp, input.Tex).x;
}



