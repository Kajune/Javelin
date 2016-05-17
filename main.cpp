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
	CMesh<> mesh;
	mesh.Initialize(loader);

	//
	// シェーダ
	//

	CVertexShader vs;
	CPixelShader ps;

	vs.Initialize("shader.hlsl", "VS", "vs_4_0");
	ps.Initialize("shader.hlsl", "PS", "ps_4_0");

	pipeline.SetVertexShader(&vs);
	pipeline.SetPixelShader(&ps);

	//
	// 入力レイアウト
	//

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0},
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
		XMFLOAT4X4 World;
		XMFLOAT4X4 ViewProjection;
	}cbObj_t;

	typedef struct {
		COLOR diffuse;
	}cbMat_t;

	CConstantBuffer<cbObj_t> cbObj;
	CConstantBuffer<cbMat_t> cbMat;
	cbObj.Initialize();
	cbMat.Initialize();

	pipeline.SetVertexShaderConstantBuffer(0, &cbObj);
	pipeline.SetPixelShaderConstantBuffer(1, &cbMat);

	cbObj_t cbObjobj;
	cbObjobj.ViewProjection = cam.GetViewProjection();

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));

		//
		// カメラ配置
		//
		cam.SetCamPos(XMFLOAT3(0.0f, 3.0f, -3.0f));
		cam.SetTargetPos(XMFLOAT3(0, 0, 0));
		cbObjobj.ViewProjection = cam.GetViewProjection();
		auto mat = XMMatrixTranspose(XMMatrixIdentity());
		XMStoreFloat4x4(&cbObjobj.World, mat);
		cbObj.UpdateBufferValue(cbObjobj, Application::GetImmediateContext());

		for (auto it = mesh.begin(); it != mesh.end(); it++) {
			pipeline.SetVertexBuffer(0, &it->vertex);
			pipeline.SetIndexBuffer(&it->index, 0);

			cbMat_t mat;
			mat.diffuse = mesh.GetMaterial(it).materialParam.diffuse;
			cbMat.UpdateBufferValue(mat, Application::GetImmediateContext());

			pipeline.SetPixelShaderResource(0, &mesh.GetMaterial(it).diffuseMap);

			pipeline.DrawIndexed(it->index.GetBufferLength());
		}
		
		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}
