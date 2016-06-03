#include "Javelin.h"
#include <random>
#include "voxel.h"

using namespace Javelin;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR lpC, int nC) {
	if (Application::Initialize("Javelin", 800, 600, false, 32)) {
		Application::Cleanup();
		return -1;
	}

	CPipeline pipeline;
	pipeline.Initialize(Application::GetImmediateContext());

	//
	// メッシュデータ
	//
	CMeshLoader<CObjLoader> loader;
	loader.Initialize("resource/cube.obj");
	CMesh<> mesh;
	mesh.Initialize(loader);

	//
	// シェーダ
	//
	CVertexShader vs;
	CPixelShader ps;

	vs.Initialize("shader.hlsl", "VS", "vs_5_0");
	ps.Initialize("shader.hlsl", "PS", "ps_5_0");
	pipeline.SetVertexShader(&vs);
	pipeline.SetPixelShader(&ps);

	//
	// 入力レイアウト
	//
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(XMFLOAT4), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(XMFLOAT4) * 2, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, sizeof(XMFLOAT4) * 3, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
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
	CCamera cam(J_PI / 3.0f, (float)width / (float)height, 0.2f, 1000.0f);

	//
	// サンプラ設定
	//
	CSamplerState sampler;
	sampler.Initialize(D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_FILTER_ANISOTROPIC, COLOR(1, 1, 1, 1));
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
		XMFLOAT4X4 ViewProjection;
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

	XMFLOAT3 LightPos(10.0f, 10.0f, -10.0f);

	cbLight_t cbLightValue;
	cbLightValue.numLights = 1;
	cbLightValue.lightPos[0] = XMFLOAT4(LightPos.x, LightPos.y, LightPos.z, 0.0f);
	cbLightValue.specularLight[0] = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	cbLightValue.diffuseLight[0] = COLOR(0.3f, 0.3f, 0.3f, 1.0f);
	cbLightValue.ambientLight = COLOR(0.8f, 0.8f, 0.8f, 1.0f).ary.rgba;
	cbLightValue.lightPower = 50.0f;

	InputMouse::SetRelativeMode(true);

	//
	//　ビューポート
	//
	CViewport viewport[2];
	D3D11_VIEWPORT vp;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.Width = (float)width / 2.0f;
	vp.Height = (float)height;
	vp.TopLeftY = 0.0f;
	for (UINT i = 0; i < 2; i++) {
		vp.TopLeftX = (float)(width * i) / 2.0f;
		viewport[i].Initialize(vp);
	}

	//
	// ボクセル
	//
	Voxel<64, 64, 32> vox(20.0f, 0.2f);
	for (unsigned int i = 16; i < 48; i++) {
		for (unsigned int j = 16; j < 48; j++) {
			for (unsigned int k = 8; k < 24; k++) {
				vox.m_cellList[i][j][k].density = 60.0f;
			}
		}
	}

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));

		//
		//　オブジェクト配置
		//
		std::vector<XMFLOAT4X4> matrix;
		for (UINT i = 0; i < vox.GetSizeX(); i++) {
			for (UINT j = 0; j < vox.GetSizeY(); j++) {
				for (UINT k = 0; k < vox.GetSizeZ(); k++) {
					if (vox.m_cellList[i][j][k].density > 0.0f
						&& (vox.m_cellList[i][j][k + 1].density <= 0.0f
							|| vox.m_cellList[i][j][k - 1].density <= 0.0f
							|| vox.m_cellList[i][j + 1][k].density <= 0.0f
							|| vox.m_cellList[i][j - 1][k].density <= 0.0f
							|| vox.m_cellList[i + 1][j][k].density <= 0.0f
							|| vox.m_cellList[i - 1][j][k].density <= 0.0f)) {
						float scale = 0.04f;
						XMFLOAT4X4 mat;
						XMStoreFloat4x4(&mat, XMMatrixTranspose(
							XMMatrixScaling(0.02f, 0.02f, 0.02f)
							* XMMatrixTranslation((float)i * scale, (float)j * scale, (float)k * scale)));
						matrix.emplace_back(mat);
					}
				}
			}
		}		
		static int count = 0;
		if (count % 1 == 0) {
			vox.Move();
		}
		count++;

		CVertexBuffer<XMFLOAT4X4> voxBuf;
		voxBuf.Initialize(matrix.size(), matrix.data(), true);
		pipeline.SetVertexBuffer(1, &voxBuf);

		//
		// カメラ配置
		//
		static float angleYaw = 0.0f, anglePitch = 0.0f;
		XMVECTOR frontVec = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
		frontVec = XMVector3Transform(frontVec, XMMatrixRotationRollPitchYaw(anglePitch, angleYaw, 0.0f));
		XMVECTOR rightVec = XMLoadFloat3(&XMFLOAT3(1, 0, 0));
		rightVec = XMVector3Transform(rightVec, XMMatrixRotationRollPitchYaw(anglePitch, angleYaw, 0.0f));
		XMVECTOR upVec = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
		upVec = XMVector3Transform(upVec, XMMatrixRotationRollPitchYaw(anglePitch, angleYaw, 0.0f));

		angleYaw += (float)InputMouse::GetRelativeMousePosX() / 500.0f
			+ InputPad::GetStickRX() / 50.0f;
		anglePitch += (float)InputMouse::GetRelativeMousePosY() / 500.0f
			+ InputPad::GetStickRY() / 50.0f;

		static XMVECTOR eyePos = XMLoadFloat3(&XMFLOAT3(0, 0, -0.5f));

		constexpr float speed = 0.01f;
		XMVECTOR actualSpeed = eyePos;

		eyePos -= frontVec * speed * InputPad::GetStickLY();
		eyePos += rightVec * speed * InputPad::GetStickLX();

		if (InputKeyboard::IsPressed('W')) {
			eyePos += frontVec * speed;
		}
		if (InputKeyboard::IsPressed('S')) {
			eyePos -= frontVec * speed;
		}
		if (InputKeyboard::IsPressed('D')) {
			eyePos += rightVec * speed;
		}
		if (InputKeyboard::IsPressed('A')) {
			eyePos -= rightVec * speed;
		}
		if (InputKeyboard::IsPressed(VK_LSHIFT)) {
			eyePos += upVec * speed;
		}
		if (InputKeyboard::IsPressed(VK_LCONTROL)) {
			eyePos -= upVec * speed;
		}
		actualSpeed = eyePos - actualSpeed;

		static float viewDiff = 0.0f;
		if (InputKeyboard::IsPressed(VK_RIGHT)) {
			viewDiff += 0.005f;
		}
		if (InputKeyboard::IsPressed(VK_LEFT)) {
			viewDiff -= 0.005f;
		}

		for (UINT i = 0; i < 2; i++) {
			XMFLOAT3 eyePosF, targetPosF;
			XMStoreFloat3(&eyePosF, eyePos + rightVec * ((float)i - 0.5f) * viewDiff);
			XMStoreFloat3(&targetPosF, eyePos + frontVec);

			cam.SetCamPos(eyePosF);
			cam.SetTargetPos(targetPosF);
			cam.SetUpVec(XMFLOAT3(0, 1, 0));

			cbObj_t cbObjValue;
			cbObjValue.ViewProjection = cam.GetViewProjection();
			cbObj.UpdateBufferValue(cbObjValue, Application::GetImmediateContext());

			cbLightValue.eyePos = eyePosF;
			cbLight.UpdateBufferValue(cbLightValue, Application::GetImmediateContext());

			//
			// 1Pass
			//
			pipeline.SetViewports(&viewport[i]);

			for (int j = 0; j < 2; j++) {
				for (auto it = mesh.begin(); it != mesh.end(); it++) {
					if ((mesh.GetMaterial(it).materialParam.opacity > 0.0f && j == 0) 
						|| (mesh.GetMaterial(it).materialParam.opacity <= 0.0f && j == 1)) {
						continue;
					}

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

					pipeline.DrawIndexedInstanced(it->index.GetBufferLength(), matrix.size());
				}
			}

		}

		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}