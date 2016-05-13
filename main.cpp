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
	// メッシュデータ
	//

	CMeshLoader<CObjLoader> loader;
	loader.Initialize("cube.obj");

	//
	// シェーダ
	//


	//
	// 入力レイアウト
	//

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CInputLayout layout;

	//
	// カメラ設定
	//

	UINT width, height;
	Application::GetScreenSize(width, height);
	CCamera cam(J_PI / 3.0f, (float)width / (float)height, 0.2f, 100.0f);

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
		cb.UpdateBufferValue(cbobj, Application::GetImmediateContext());	
		
		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}
