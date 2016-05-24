cbuffer cbObj : register(b0) {
	matrix ViewProjection;
};

SamplerState smp : register(s0);

struct VS_INPUT {
	float3 position	: POSITION;
};

struct GS_INPUT {
	float4 position : SV_POSITION;
};

struct PS_INPUT {
	float4 position	: SV_POSITION;
//	float4 posWorld	: POSITION;
//	float4 normal	: NORMAL;
};

GS_INPUT VS(VS_INPUT input) {
	GS_INPUT output;
	output.position = mul(float4(input.position, 1.0), ViewProjection);
	return output;
}

[maxvertexcount(1)]
void GS(point GS_INPUT In[1],
	inout PointStream<PS_INPUT> Stream) {
	PS_INPUT output;
	output.position = In[0].position;
	Stream.Append(output);
}

float4 PS(PS_INPUT input) : SV_TARGET{
	return float4(1, 1, 1, 1);
}
