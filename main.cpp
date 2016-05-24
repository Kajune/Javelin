#include "Javelin.h"
#include <random>
#include "voxel.h"

using namespace Javelin;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR lpC, int nC) {
	if (Application::Initialize("Javelin", 800, 600, true, 1)) {
		Application::Cleanup();
		return -1;
	}

	CPipeline pipeline;
	pipeline.Initialize(Application::GetImmediateContext());

	constexpr UINT kMaxVoxelNum = 10000;

	//
	// 頂点バッファ
	//

	CDynamicVertexBuffer<XMFLOAT3> vertex;
	vertex.Initialize(kMaxVoxelNum, nullptr);
	pipeline.SetVertexBuffer(0, &vertex);

	//
	// シェーダ
	//

	CVertexShader vs;
	CGeometryShader gs;
	CPixelShader ps;

	vs.Initialize("shader.hlsl", "VS", "vs_4_0");
	gs.Initialize("shader.hlsl", "GS", "gs_4_0");
	ps.Initialize("shader.hlsl", "PS", "ps_4_0");

	pipeline.SetVertexShader(&vs);
	pipeline.SetGeometryShader(&gs);
	pipeline.SetPixelShader(&ps);

	//
	// 入力レイアウト
	//

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	CInputLayout layout;
	layout.Initialize(inputDesc, array_length(inputDesc), vs);

	pipeline.SetInputLayout(&layout);

	pipeline.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//
	// カメラ設定
	//

	UINT width, height;
	Application::GetScreenSize(width, height);
	CCamera cam(J_PI / 3.0f, (float)width / (float)height, 0.01f, 1000.0f);

	//
	// サンプラ設定
	//

	CSamplerState sampler;
	sampler.Initialize(D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_FILTER_ANISOTROPIC, COLOR(1, 1, 1, 1));
	pipeline.SetPixelShaderSamplerState(0, &sampler);

	//
	// 定数バッファ設定
	//

	typedef struct {
		XMFLOAT4X4 ViewProjection;
	}cbObj_t;

	CConstantBuffer<cbObj_t> cbObj;
	cbObj.Initialize();

	pipeline.SetVertexShaderConstantBuffer(0, &cbObj);

	InputMouse::SetRelativeMode(true);

	//
	// ボクセル
	//

	Voxel<30, 30, 30> vox(20.0f, 0.2f);
	for (unsigned int i = 10; i < 20; i++) {
		for (unsigned int j = 10; j < 20; j++) {
			for (unsigned int k = 10; k < 20; k++) {
				vox.m_cellList[i][j][k].density = 60.0f;
			}
		}
	}

	int count = 0;
	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));

		//
		// カメラ配置
		//
		cbObj_t cbObjValue;
		static float angleYaw = 0.0f, anglePitch = 0.0f;
		XMVECTOR frontVec = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
		frontVec = XMVector3Transform(frontVec, XMMatrixRotationRollPitchYaw(anglePitch, angleYaw, 0.0f));
		XMVECTOR rightVec = XMLoadFloat3(&XMFLOAT3(1, 0, 0));
		rightVec = XMVector3Transform(rightVec, XMMatrixRotationRollPitchYaw(anglePitch, angleYaw, 0.0f));
		XMVECTOR upVec = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
		upVec = XMVector3Transform(upVec, XMMatrixRotationRollPitchYaw(anglePitch, angleYaw, 0.0f));

		angleYaw += InputMouse::GetRelativeMousePosX() / 500.0f;
		anglePitch += InputMouse::GetRelativeMousePosY() / 500.0f;

		static XMVECTOR eyePos = XMLoadFloat3(&XMFLOAT3(0, 0, -0.5f));

		constexpr float speed = 0.01f;
		XMVECTOR actualSpeed = eyePos;
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

		XMFLOAT3 eyePosF, targetPosF;
		XMStoreFloat3(&eyePosF, eyePos);
		XMStoreFloat3(&targetPosF, eyePos + frontVec);

		cam.SetCamPos(eyePosF);
		cam.SetTargetPos(targetPosF);
		cam.SetUpVec(XMFLOAT3(0, 1, 0));
		cbObjValue.ViewProjection = cam.GetViewProjection();

		cbObj.UpdateBufferValue(cbObjValue, Application::GetImmediateContext());

		std::vector<XMFLOAT3> vertexList;
		for (unsigned int i = 1; i < 29; i++) {
			for (unsigned int j = 1; j < 29; j++) {
				for (unsigned int k = 1; k < 29; k++) {
					if (vox.m_cellList[i][j][k].density > 0) {
						if (vox.m_cellList[i - 1][j][k].density <= 0 ||
							vox.m_cellList[i][j - 1][k].density <= 0 ||
							vox.m_cellList[i][j][k - 1].density <= 0 ||
							vox.m_cellList[i + 1][j][k].density <= 0 ||
							vox.m_cellList[i][j + 1][k].density <= 0 ||
							vox.m_cellList[i][j][k + 1].density <= 0) {
							constexpr float scale = 0.01f;
							vertexList.emplace_back(((float)i - 15.0f)* scale,
								((float)j - 15.0f) * scale, ((float)k - 15.0f) * scale);
							cbObj.UpdateBufferValue(cbObjValue, Application::GetImmediateContext());
						}
					}
				}
			}
		}
		vertex.WriteBuffer(vertexList.size(), vertexList.data(), Application::GetImmediateContext());

		pipeline.Draw(vertexList.size());

		if (count++ > 300 && count % 10 == 0) {
			vox.Move();
		}

		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}
