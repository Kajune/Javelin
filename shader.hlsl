SamplerState smp : register(s0);

cbuffer cbNeverChanges : register(b0) {
	matrix Projection;
};

cbuffer cbChangesEveryFrame : register(b1) {
	matrix View;
	matrix ViewCube[6];
	float3 Light;
};

static const int numInstance = 64;

cbuffer cbChangesEveryObject :register(b2) {
	matrix World[numInstance];
};

struct VS_INPUT {
	float3 Pos : POSITION;
	float3 Col : COLOR;
};

struct GS_INPUT {
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
};

struct PS_INPUT {
	float4 Pos : SV_POSITION;
	float3 PosView : POSVIEW;
	float3 Norm : NORMAL;
	float4 Col : COLOR;
};

GS_INPUT VS(VS_INPUT input, uint instID : SV_InstanceID) {
	GS_INPUT output;
	float4 pos4 = float4(input.Pos, 1.0);
	output.Pos = mul(mul(pos4, World[instID % numInstance]), View);
	output.Col = float4(input.Col, 1.0);

	return output;
}

[maxvertexcount(3)]
void GS(triangle GS_INPUT input[3],
	inout TriangleStream<PS_INPUT> TriStream) {
	PS_INPUT output;

	float3 faceEdge = input[0].Pos.xyz / input[0].Pos.w;
	float3 faceEdgeA = (input[1].Pos.xyz / input[1].Pos.w) - faceEdge;
	float3 faceEdgeB = (input[2].Pos.xyz / input[2].Pos.w) - faceEdge;
	output.Norm = normalize(cross(faceEdgeA, faceEdgeB));

	for (int i = 0; i < 3; i++) {
		output.PosView = input[i].Pos.xyz / input[i].Pos.w;
		output.Pos = mul(input[i].Pos, Projection);
		output.Col = input[i].Col;

		TriStream.Append(output);
	}

	TriStream.RestartStrip();
}

float4 PS(PS_INPUT input) : SV_TARGET{
	float3 light = Light - input.PosView;
	float bright = 1000 * dot(normalize(light), input.Norm) / pow(length(light), 2);
	return saturate(bright * input.Col);
}

//
//　キューブ描画
//

struct VS_OUTPUT_CUBEMAP {
	float4 Pos : SV_POSITION;
	float4 Col : COLOR;
};

struct GS_OUTPUT_CUBEMAP {
	float4 Pos : SV_POSITION;
	float3 PosView : POSVIEW;
	float3 Norm : NORMAL;
	float4 Col : COLOR;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

VS_OUTPUT_CUBEMAP VS_CubeMap(VS_INPUT input, uint instID : SV_InstanceID) {
	VS_OUTPUT_CUBEMAP output;
	output.Pos = mul(float4(input.Pos, 1.0), World[instID % numInstance]);
	output.Col = float4(input.Col, 1.0);
	return output;
}

[maxvertexcount(18)]
void GS_CubeMap(triangle VS_OUTPUT_CUBEMAP In[3],
	inout TriangleStream<GS_OUTPUT_CUBEMAP> CubeMapStream) {

	for (int f = 0; f < 6; f++) {
		GS_OUTPUT_CUBEMAP output;
		float3 faceEdge = In[0].Pos.xyz / In[0].Pos.w;
		float3 faceEdgeA = (In[1].Pos.xyz / In[1].Pos.w) - faceEdge;
		float3 faceEdgeB = (In[2].Pos.xyz / In[2].Pos.w) - faceEdge;
		output.Norm = normalize(cross(faceEdgeA, faceEdgeB));

		output.RTIndex = f;
		for (int v = 0; v < 3; v++) {
			output.Pos = mul(In[v].Pos, ViewCube[f]);
			output.PosView = In[v].Pos.xyz / In[v].Pos.w;
			output.Pos = mul(output.Pos, Projection);
			output.Col = In[v].Col;
			CubeMapStream.Append(output);
		}
		CubeMapStream.RestartStrip();
	}
}
