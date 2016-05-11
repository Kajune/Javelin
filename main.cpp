#include "Javelin.h"
#include <random>

using namespace Javelin;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR lpC, int nC) {
	if (Application::Initialize("Javelin", 800, 600, true, 32)) {
		Application::Cleanup();
		return -1;
	}

	CPipeline pipeline;
	pipeline.Initialize(Application::GetImmediateContext());

	//
	// 頂点バッファ
	//

	typedef struct {
		XMFLOAT3 pos;
		XMFLOAT3 velocity;
		XMFLOAT3 velocityFirst;
	}vertex;

	CVertexBuffer<vertex> vb[2];

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist1(-3.0f, 3.0f);
	std::uniform_real_distribution<float> dist2(0.0f, 5.0f);
	vertex pVertices[128];
	for (auto& vertex : pVertices) {
		vertex.pos = XMFLOAT3(0, 0.1f, 0);
		vertex.velocity = XMFLOAT3(dist1(mt), dist2(mt), dist1(mt));
		vertex.velocityFirst = vertex.velocity;
	}
	for (int i = 0; i < 2; i++) {
		vb[i].Initialize(array_length(pVertices), pVertices, true);
	}

	//
	// シェーダ
	//

	CVertexShader vs_particle_vertex;
	vs_particle_vertex.Initialize("shader.hlsl", "VS_Particle_Vertex", "vs_4_0");
	D3D11_SO_DECLARATION_ENTRY decl[] = {
		{ 0, "POSITION", 0, 0, 3, 0 },
		{ 0, "NORMAL", 0, 0, 3, 0 },
		{ 0, "TEXCOORD", 0, 0, 3, 0 },
	};
	UINT bufferStrides[] = { sizeof(vertex) };
	CGeometryShader gs_particle_vertex;
	gs_particle_vertex.Initialize("shader.hlsl", "GS_Particle_Vertex", "gs_4_0", 
		decl, array_length(decl), bufferStrides, array_length(bufferStrides));

	CGeometryShader gs_particle;
	gs_particle.Initialize("shader.hlsl", "GS_Particle", "gs_4_0");
	CPixelShader ps_particle;
	ps_particle.Initialize("shader.hlsl", "PS_Particle", "ps_4_0");

	CVertexShader vs_floor;
	vs_floor.Initialize("shader.hlsl", "VS_Floor", "vs_4_0");
	CPixelShader ps_floor;
	ps_floor.Initialize("shader.hlsl", "PS_Floor", "ps_4_0");

	//
	// 入力レイアウト
	//

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CInputLayout layout;
	layout.Initialize(inputDesc, array_length(inputDesc), vs_particle_vertex);

	//
	//　デプスステンシルステート
	//

	CDepthStencilState dsState;
	D3D11_DEPTH_STENCIL_DESC desc{};
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = false;
	dsState.Initialize(desc);

	//
	// カメラ設定
	//

	UINT width, height;
	Application::GetScreenSize(width, height);
	CCamera cam(J_PI / 3.0f, (float)width / (float)height, 0.2f, 100.0f);

	//
	//　テクスチャ用意
	//

	CImage particleTex, floorTex;
	particleTex.Initialize("particle.png");
	floorTex.Initialize("floor.png");

	pipeline.SetPixelShaderResource(0, &particleTex);
	pipeline.SetPixelShaderResource(1, &floorTex);

	//
	//　ブレンドステート設定
	//

	CBlendState blState;
	blState.Initialize();
	blState.SetAddBlend();
	blState.ApplyChange();

	//
	// サンプラ設定
	//

	CSamplerState sampler;
	sampler.Initialize(D3D11_TEXTURE_ADDRESS_WRAP);
	pipeline.SetPixelShaderSamplerState(0, &sampler);

	//
	// 定数バッファ設定
	//

	typedef struct {
		XMFLOAT4X4 view;
		XMFLOAT4X4 Projection;
		XMFLOAT2 ParticleSize;
		XMFLOAT2 Dummy1;
		float Time;
		XMFLOAT3 Dummy2;
	}cb_t;

	CConstantBuffer<cb_t> cb;
	cb.Initialize();

	pipeline.SetVertexShaderConstantBuffer(0, &cb);
	pipeline.SetGeometryShaderConstantBuffer(0, &cb);

	cb_t cbobj;
	cbobj.Projection = cam.GetProjection();
	cbobj.ParticleSize = XMFLOAT2(0.04f, 0.04f);

	//
	// ウィンドウサイズ変更時のコールバック登録
	//

	Application::SetCallbackFuncWhenScreensizeChanged([&](UINT width, UINT height) {
		cam.SetAspect((float)width / (float)height);
		cbobj.Projection = cam.GetProjection();
	});

	UINT soTargetIndex = 0;

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));

		//
		// カメラ配置
		//
		cam.SetCamPos(XMFLOAT3(0.0f, 3.0f, -3.0f));
		cam.SetTargetPos(XMFLOAT3(0, 0, 0));
		cbobj.view = cam.GetView();
		static DWORD before_time = GetTickCount();
		DWORD now_time = GetTickCount();
		cbobj.Time = (now_time - before_time) / 1000.0f;
		before_time = now_time;
		cb.UpdateBufferValue(cbobj, Application::GetImmediateContext());
		
		//
		// 描画(1Pass)
		//

		pipeline.SetInputLayout(nullptr);
		pipeline.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pipeline.SetVertexShader(&vs_floor);
		pipeline.SetGeometryShader(nullptr);
		pipeline.SetPixelShader(&ps_floor);

		pipeline.Draw(6);

		//
		// 描画(2Pass)
		//

		pipeline.SetInputLayout(&layout);
		pipeline.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		pipeline.SetVertexBuffer(0, &vb[soTargetIndex]);

		pipeline.SetStreamOutputTarget(&vb[(soTargetIndex + 1) % 2], 0);

		pipeline.SetVertexShader(&vs_particle_vertex);
		pipeline.SetGeometryShader(&gs_particle_vertex);
		pipeline.SetPixelShader(nullptr);

		pipeline.SetDepthStencilState(&dsState, 0);

		static bool first = true;
		if (first) {
			pipeline.Draw(vb[soTargetIndex].GetBufferLength());
			first = false;
		} else {
			pipeline.DrawAuto();
		}

		pipeline.SetStreamOutputTarget(nullptr, 0);

		soTargetIndex = ++soTargetIndex % 2;

		//
		// 描画(3Pass)
		//

		pipeline.SetVertexBuffer(0, &vb[soTargetIndex]);

		pipeline.SetVertexShader(&vs_particle_vertex);
		pipeline.SetGeometryShader(&gs_particle);
		pipeline.SetPixelShader(&ps_particle);

		pipeline.SetBlendState(&blState);

		pipeline.DrawAuto();

		Application::SetDefaultDepthStencilState(pipeline);
		Application::SetDefaultBlendState(pipeline);

		
		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}
