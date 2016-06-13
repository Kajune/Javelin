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

	pipeline.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//
	// カメラ設定
	//

	UINT width, height;
	Application::GetScreenSize(width, height);
	CCamera cam(J_PI / 3.0f, (float)width / (float)height, 0.2f, 1000.0f);

	//
	// 定数バッファ設定
	//

	float LightPower[] = { 20.0f };
	Graphics3D::SetLightParam(1, &XMFLOAT3(5.0f, 5.0f, -5.0f),
		&COLOR(1.0f, 1.0f, 1.0f, 1.0f),
		&COLOR(1.0f, 1.0f, 1.0f, 1.0f), 
		LightPower, COLOR(0.5f, 0.5f, 0.5f, 1.0f));

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));
		Graphics3D::ClearShadowBuffer();

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

		static XMVECTOR eyePos;

		constexpr float speed = 0.1f;
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

		XMFLOAT3 eyePosF, targetPosF;
		XMStoreFloat3(&eyePosF, eyePos);
		XMStoreFloat3(&targetPosF, eyePos + frontVec);

		cam.SetCamPos(eyePosF);
		cam.SetTargetPos(targetPosF);
		cam.SetUpVec(XMFLOAT3(0, 1, 0));

		static float angle = 0.0f;
		auto mat = XMMatrixTranspose(
			XMMatrixScaling(0.02f, 0.02f, 0.02f)
			* XMMatrixRotationY(angle));
		angle += 0.01f;
		XMFLOAT4X4 mat_f;
		XMStoreFloat4x4(&mat_f, mat);

		Graphics3D::SetShadowCameraTransform(XMFLOAT3(5.0f, 5.0f, -5.0f), targetPosF);

		//
		// 1Pass
		//

		Graphics3D::DrawShadowBuffer(mesh, cam, mat_f, pipeline);

		//
		// 2Pass
		//

		Graphics3D::DrawMesh(mesh, cam, mat_f, pipeline);

		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}