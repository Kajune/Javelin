#include "Javelin.h"

using namespace Javelin;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR lpC, int nC) {
	if (Application::Initialize("Javelin", 800, 600, true, 32)){
		Application::Cleanup();
		return -1;
	}

	CPipeline pipeline;
	pipeline.Initialize(Application::GetImmediateContext());

	//
	// キューブの頂点バッファ
	//

	typedef struct {
		XMFLOAT3 pos;
		XMFLOAT3 col;
		XMFLOAT2 tex;
	}vertex;

	CVertexBuffer<vertex> vb;
	vertex pVertices[] = {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)},
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
	};
	vb.Initialize(array_length(pVertices), pVertices);

	pipeline.SetVertexBuffer(0, &vb);

	//
	// シェーダ
	//

	CVertexShader vs;
	vs.Initialize("shader.hlsl", "VS", "vs_4_0");
	CGeometryShader gs;
	gs.Initialize("shader.hlsl", "GS", "gs_4_0");
	CPixelShader ps;
	ps.Initialize("shader.hlsl", "PS", "ps_4_0");

	pipeline.SetVertexShader(&vs);
	pipeline.SetGeometryShader(&gs);
	pipeline.SetPixelShader(&ps);

	//
	// 入力レイアウト
	//

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CInputLayout layout;
	layout.Initialize(inputDesc, array_length(inputDesc), vs);

	pipeline.SetInputLayout(&layout);
	pipeline.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	// カメラ設定
	//

	UINT width, height;
	Application::GetScreenSize(width, height);
	CCamera cam(J_PI / 6.0f, (float)width / (float)height, 1.0f, 100.0f);

	//
	// 定数バッファ設定
	//

	typedef struct {
		XMFLOAT4X4 view;
		XMFLOAT3 light;
		FLOAT dummy;
	}cbChangesEveryFrame_t;

	constexpr int num_inst = 64;
	CConstantBuffer<XMFLOAT4X4> cbNeverChanges;
	CConstantBuffer<cbChangesEveryFrame_t> cbChangesEveryFrame;
	CConstantBuffer<XMFLOAT4X4[num_inst]> cbChangesEveryObject;

	cbNeverChanges.Initialize();
	cbChangesEveryFrame.Initialize();
	cbChangesEveryObject.Initialize();

	pipeline.SetGeometryShaderConstantBuffer(0, &cbNeverChanges);

	pipeline.SetVertexShaderConstantBuffer(1, &cbChangesEveryFrame);
	pipeline.SetPixelShaderConstantBuffer(1, &cbChangesEveryFrame);

	pipeline.SetVertexShaderConstantBuffer(2, &cbChangesEveryObject);

	cbNeverChanges.UpdateBufferValue(cam.GetProjection(), Application::GetImmediateContext());

	//
	// ウィンドウサイズ変更時のコールバック登録
	//

	Application::SetCallbackFuncWhenScreensizeChanged([&](UINT width, UINT height) {
		cam.SetAspect((float)width / (float)height);
		cbNeverChanges.UpdateBufferValue(cam.GetProjection(), Application::GetImmediateContext());
	});

	//
	// サンプラ設定
	//

	CSamplerState sampler;
	sampler.Initialize(D3D11_TEXTURE_ADDRESS_WRAP);
	pipeline.SetPixelShaderSamplerState(0, &sampler);

	//
	// キューブテクスチャ設定
	//

	CCubeTexture cubeTex;
	cubeTex.Initialize(512, 512, DXGI_FORMAT_R16G16B16A16_FLOAT, 9);

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));
		
		//
		// カメラ配置
		//
		cam.SetCamPos(XMFLOAT3(0, 10, -30));
		cam.SetTargetPos(XMFLOAT3(0, 0, 0));
		cbChangesEveryFrame.UpdateBufferValue(cbChangesEveryFrame_t{ cam.GetView(), XMFLOAT3(1, 1, -2) }, 
			Application::GetImmediateContext());

		//
		// オブジェクト配置
		//
		XMMATRIX matX, matY, matZ;
		float rotate = (float)(J_PI * (timeGetTime())) / 3000.0f;
		matZ = XMMatrixRotationZ(rotate);
		rotate = (float)(J_PI * (timeGetTime())) / 1500.0f;
		matX = XMMatrixRotationX(rotate);
		rotate = (float)(J_PI * (timeGetTime())) / 6000.0f;
		matY = XMMatrixRotationY(rotate);

		XMFLOAT4X4 world[num_inst];
		constexpr float radius = 8.0f;
		for (UINT i = 0; i < num_inst; i++) {
			float yaw = (float)(i / int(sqrt((float)num_inst))) * J_PI * 2.0f / sqrt((float)num_inst);
			float pitch = (float)i * J_PI / sqrtf((float)num_inst) + J_PI / 4.0f;
			XMStoreFloat4x4(&world[i],
				XMMatrixTranspose(matZ * matX 
					* XMMatrixTranslation(
						radius * cosf(yaw) * cosf(pitch),
						radius * sinf(pitch),
						radius * sinf(yaw) * cosf(pitch)) * matY));
		}

		cbChangesEveryObject.UpdateBufferValue(world, Application::GetImmediateContext());

		//
		// 描画(1Pass)
		//
		pipeline.DrawInstanced(vb.GetBufferLength(), num_inst);

		//
		// 描画(2Pass)
		//

		//
		// 描画(3Pass)
		//
		
		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}
