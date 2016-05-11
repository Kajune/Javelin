cbuffer cb : register(b0) {
	matrix View;
	matrix Projection;

	float2 ParticleSize;
	float2 dummy;
	float Time;
};

static float3 Acceleration = float3(0.0, -9.885, 0.0);

Texture2D TexParticle : register(t0);
Texture2D TexFloor : register(t1);
SamplerState smp : register(s0);

//
//　パーティクル座標更新
//

struct GS_INPUT_PARTICLE {
	float3 Position		: POSITION;
	float3 Velocity		: NORMAL;
	float3 VelocityFirst: TEXCOORD0;
};

GS_INPUT_PARTICLE VS_Particle_Vertex(GS_INPUT_PARTICLE In) {
	return In;
}

[maxvertexcount(1)]
void GS_Particle_Vertex(point GS_INPUT_PARTICLE In[1],
	inout PointStream<GS_INPUT_PARTICLE> ParticleStream) {
	GS_INPUT_PARTICLE output;
	output.VelocityFirst = In[0].VelocityFirst;
	output.Velocity = Acceleration * Time + In[0].Velocity;
	output.Position = 0.5 * Acceleration * Time * Time + In[0].Velocity * Time + In[0].Position;
	if (output.Position.y < 0) {
		if (abs(output.Velocity.y) < 0.1) {
			output.Position = float3(0.0, 0.1, 0.0);
			output.Velocity = In[0].VelocityFirst;
		} else {
			output.Velocity.y = abs(output.Velocity.y) / 1.2;
		}
	}
	ParticleStream.Append(output);
}

//
//　パーティクル描画
//

struct PS_INPUT {
	float4 Pos	: SV_POSITION;
	float2 Tex	: TEXTURE;
};

static const float2 signList[] = {
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

static const float2 texList[] = {
	float2(0.0, 0.0),
	float2(1.0, 0.0),
	float2(0.0, 1.0),
	float2(1.0, 1.0),
};

[maxvertexcount(4)]
void GS_Particle(point GS_INPUT_PARTICLE In[1],
	inout TriangleStream<PS_INPUT> ParticleStream) {
	float4 pos = mul(float4(In[0].Position, 1.0), View);

	for (int i = 0; i < 4; i++) {
		PS_INPUT output;
		output.Pos = mul(pos + float4(ParticleSize * signList[i], 0.0, 0.0) * pos.w, Projection);
		output.Tex = texList[i];
		ParticleStream.Append(output);
	}
	ParticleStream.RestartStrip();
}

float4 PS_Particle(PS_INPUT input) : SV_Target{
	return TexParticle.Sample(smp, input.Tex);
}

//
//　床描画
//

static const float4 vertexList[] = {
	float4(-2, 0, -2, 1),
	float4(-2, 0, 2, 1),
	float4(2, 0, 2, 1),
	float4(2, 0, 2, 1),
	float4(2, 0, -2, 1),
	float4(-2, 0, -2, 1),
};

static const float2 texListFloor[] = {
	float2(0, 1),
	float2(0, 0),
	float2(1, 0),
	float2(1, 0),
	float2(1, 1),
	float2(0, 1),
};

PS_INPUT VS_Floor(uint vid : SV_VertexID) {
	PS_INPUT output;
	output.Pos = mul(mul(vertexList[vid], View), Projection);
	output.Tex = texListFloor[vid];
	return output;
}

float4 PS_Floor(PS_INPUT input) : SV_Target{
	return TexFloor.Sample(smp, input.Tex) * 0.5;
}
