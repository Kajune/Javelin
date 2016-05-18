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
	loader.Initialize("resource/akagi.obj");
	CMesh<> mesh;
	mesh.Initialize(loader);
	struct alloc {
		static XMFLOAT3 Convert(const vertex_t& vertex) {
			return vertex.position;
		}
	};
	CMesh<XMFLOAT3,alloc> mesh_shadow;
	mesh_shadow.Initialize(loader);

	//
	// シェーダ
	//

	CVertexShader vs, vs_shadow;
	CPixelShader ps;

	vs.Initialize("shader.hlsl", "VS", "vs_4_0");
	vs_shadow.Initialize("shader.hlsl", "VS_Shadow", "vs_4_0");
	ps.Initialize("shader.hlsl", "PS", "ps_4_0");

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

	D3D11_INPUT_ELEMENT_DESC inputDescShadow[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	CInputLayout layoutShadow;
	layoutShadow.Initialize(inputDescShadow, array_length(inputDescShadow), vs_shadow);

	pipeline.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	//　深度バッファ
	//

	constexpr int smSize = 2048;
	CDepthStencil dsShadow;
	dsShadow.Initialize(smSize, smSize);

	//
	//　ビューポート
	//

	CViewport vpShadow;
	vpShadow.Initialize(smSize, smSize);

	//
	// カメラ設定
	//

	UINT width, height;
	Application::GetScreenSize(width, height);
	CCamera cam(J_PI / 3.0f, (float)width / (float)height, 0.2f, 1000.0f);
	CCamera camShadow(J_PI / 3.0f, 1.0f, 0.2f, 1000.0f);

	//
	// サンプラ設定
	//

	CSamplerState sampler;
	sampler.Initialize(D3D11_TEXTURE_ADDRESS_WRAP);
	pipeline.SetPixelShaderSamplerState(0, &sampler);

	//
	//　デフォルトテクスチャ
	//

	CImage defaultWhite;
	defaultWhite.Initialize("resource/white.png");

	//
	// 定数バッファ設定
	//

	typedef struct {
		XMFLOAT4X4 World;
		XMFLOAT4X4 ViewProjection;
		XMFLOAT4X4 ViewProjectionShadow;
	}cbObj_t;

	typedef struct {
		COLOR diffuse, specular, ambient, opacity_reflection_refraction;
	}cbMat_t;

	constexpr UINT MaxLightNum = 16;
	typedef struct {
		XMFLOAT3 eyePos;
		UINT numLights;
		XMFLOAT3 ambientLight;
		float lightPower;
		XMFLOAT4 lightPos[MaxLightNum];
		COLOR specularLight[MaxLightNum];
		COLOR diffuseLight[MaxLightNum];
	}cbLight_t;

	CConstantBuffer<cbObj_t> cbObj;
	CConstantBuffer<cbMat_t> cbMat;
	CConstantBuffer<cbLight_t> cbLight;
	cbObj.Initialize();
	cbMat.Initialize();
	cbLight.Initialize();

	pipeline.SetVertexShaderConstantBuffer(0, &cbObj);
	pipeline.SetPixelShaderConstantBuffer(1, &cbMat);
	pipeline.SetPixelShaderConstantBuffer(2, &cbLight);

	XMFLOAT3 LightPos(5.0f, 5.0f, -5.0f);

	cbLight_t cbLightValue;
	cbLightValue.numLights = 1;
	cbLightValue.lightPos[0] = XMFLOAT4(LightPos.x, LightPos.y, LightPos.z, 0.0f);
	cbLightValue.specularLight[0] = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	cbLightValue.diffuseLight[0] = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	cbLightValue.ambientLight = COLOR(0.5f, 0.5f, 0.5f, 1.0f).ary.rgba;
	cbLightValue.lightPower = 20.0f;


	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));
		Application::ClearScreen(nullptr, &dsShadow, COLOR(1.0f, 1.0f, 1.0f, 1.0f));

		//
		// カメラ配置
		//
		cbObj_t cbObjValue;
		XMFLOAT3 eyePos(0.0f, 3.0f, -3.0f);
		XMFLOAT3 targetPos(0, 0, 0);

		cam.SetCamPos(eyePos);
		cam.SetTargetPos(targetPos);
		cbObjValue.ViewProjection = cam.GetViewProjection();

		camShadow.SetCamPos(LightPos);
		camShadow.SetTargetPos(targetPos);
		cbObjValue.ViewProjectionShadow = camShadow.GetViewProjection();

		cbObj.UpdateBufferValue(cbObjValue, Application::GetImmediateContext());

		auto mat = XMMatrixTranspose(
			XMMatrixScaling(0.02f, 0.02f, 0.02f)
			* XMMatrixRotationY(timeGetTime() / 1000.0f) 
			* XMMatrixRotationX(J_PI / 12.0f));
		XMStoreFloat4x4(&cbObjValue.World, mat);

		cbObj.UpdateBufferValue(cbObjValue, Application::GetImmediateContext());

		cbLightValue.eyePos = eyePos;
		cbLight.UpdateBufferValue(cbLightValue, Application::GetImmediateContext());

		//
		// 1Pass
		//

		pipeline.SetPixelShaderResource(7, nullptr);

		pipeline.SetInputLayout(&layoutShadow);

		pipeline.SetVertexShader(&vs_shadow);
		pipeline.SetPixelShader(nullptr);

		pipeline.SetRenderTarget(nullptr, &dsShadow);
		pipeline.SetViewports(vpShadow);

		for (auto it = mesh_shadow.begin(); it != mesh_shadow.end(); it++) {
			pipeline.SetVertexBuffer(0, &it->vertex);
			pipeline.SetIndexBuffer(&it->index, 0);

			pipeline.DrawIndexed(it->index.GetBufferLength());
		}

		//
		// 2Pass
		//

		pipeline.SetInputLayout(&layout);

		pipeline.SetVertexShader(&vs);
		pipeline.SetPixelShader(&ps);

		Application::SetDefaultRenderTarget(pipeline);
		Application::SetDefaultViewport(pipeline);

		pipeline.SetPixelShaderResource(7, &dsShadow);

		for (auto it = mesh.begin(); it != mesh.end(); it++) {
			pipeline.SetVertexBuffer(0, &it->vertex);
			pipeline.SetIndexBuffer(&it->index, 0);

			cbMat_t mat;
			mat.diffuse = mesh.GetMaterial(it).materialParam.diffuse;
			mat.specular = mesh.GetMaterial(it).materialParam.specular;
			mat.ambient = mesh.GetMaterial(it).materialParam.ambient;
			mat.opacity_reflection_refraction 
				= COLOR(mesh.GetMaterial(it).materialParam.opacity,
					mesh.GetMaterial(it).materialParam.reflection,
					mesh.GetMaterial(it).materialParam.refraction, 0.0f);
			cbMat.UpdateBufferValue(mat, Application::GetImmediateContext());

			pipeline.SetPixelShaderResource(0, 
				(mesh.GetMaterial(it).diffuseMap ? &mesh.GetMaterial(it).diffuseMap : &defaultWhite));
			pipeline.SetPixelShaderResource(1,
				(mesh.GetMaterial(it).specularMap ? &mesh.GetMaterial(it).specularMap : 
				(mesh.GetMaterial(it).diffuseMap ? &mesh.GetMaterial(it).diffuseMap : &defaultWhite)));
			pipeline.SetPixelShaderResource(2,
				(mesh.GetMaterial(it).ambientMap ? &mesh.GetMaterial(it).ambientMap :
				(mesh.GetMaterial(it).diffuseMap ? &mesh.GetMaterial(it).diffuseMap : &defaultWhite)));

			pipeline.DrawIndexed(it->index.GetBufferLength());
		}
		
		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}
