#include "Javelin.h"
#include <random>

using namespace Javelin;

XMVECTOR CalcScreenToWorld(
	int Sx,  // スクリーンX座標
	int Sy,  // スクリーンY座標
	float fZ,  // 射影空間でのZ値（0～1）
	int Screen_w, int Screen_h,
	const XMMATRIX& View, const XMMATRIX& Proj) {
	XMMATRIX InvView, InvProj, VP, InvViewport;
	InvView = XMMatrixInverse(&XMMatrixDeterminant(View), View);
	InvProj = XMMatrixInverse(&XMMatrixDeterminant(Proj), Proj);
	VP = XMMatrixIdentity();
	VP._11 = Screen_w / 2.0f; VP._22 = -Screen_h / 2.0f;
	VP._41 = Screen_w / 2.0f; VP._42 = Screen_h / 2.0f;
	InvViewport = XMMatrixInverse(&XMMatrixDeterminant(VP), VP);

	return XMVector3TransformCoord(
		XMLoadFloat3(&XMFLOAT3((float)Sx, (float)Sy, (float)fZ)), 
		InvViewport * InvProj * InvView);
}

XMVECTOR CalcScreenToXZ(
	int Sx, int Sy,
	int Screen_w, int Screen_h,
	const XMMATRIX& View, const XMMATRIX& Prj) {
	auto nearpos = CalcScreenToWorld(Sx, Sy, 0.0f, Screen_w, Screen_h, View, Prj);
	auto farpos = CalcScreenToWorld(Sx, Sy, 1.0f, Screen_w, Screen_h, View, Prj);
	auto ray = XMVector3Normalize(farpos - nearpos);
	XMFLOAT3 ray_f;
	XMStoreFloat3(&ray_f, ray);

	XMVECTOR ret;
	if (ray_f.y <= 0) {
		XMFLOAT3 tmp;
		XMStoreFloat3(&tmp, XMVector3Dot(ray, XMLoadFloat3(&XMFLOAT3(0, 1, 0))));
		float Lray = tmp.x;
		XMStoreFloat3(&tmp, XMVector3Dot(-nearpos, XMLoadFloat3(&XMFLOAT3(0, 1, 0))));
		float LP0 = tmp.x;
		ret = nearpos + (LP0 / Lray) * ray;
	} else {
		ret = farpos;
	}

	return ret;
}

XMFLOAT3 R(const XMFLOAT2& uv, const XMFLOAT3 P[4]) {
	if (uv.x < 0 || 1 < uv.x || uv.y < 0 || 1 < uv.y) {
		return XMFLOAT3(0, 0, 0);
	}
	XMVECTOR P_[4];
	for (UINT i = 0; i < 4; i++) {
		P_[i] = XMLoadFloat3(&P[i]);
	}

	XMFLOAT3 ret;
	XMStoreFloat3(&ret,
		(1.0f - uv.x) * (1.0f - uv.y) * P_[2]
		+ uv.x * (1.0f - uv.y) * P_[0]
		+ (1.0f - uv.x) * uv.y * P_[3]
		+ uv.x * uv.y * P_[1]);
	return ret;
}

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
	constexpr float CellSize = 0.1f;
	constexpr UINT SizeX = 100, SizeY = 100;
	XMFLOAT3 Map[SizeY + 1][SizeX];
	UINT Index[SizeX * SizeY * 2];

	for (UINT x = 0; x < SizeX; x++) {
		for (UINT y = 0; y <= SizeY; y++) {
			Map[y][x] = XMFLOAT3((float)x * CellSize, 0, (float)y * CellSize);
		}
	}
	
	UINT index = 0;
	for (UINT y = 0; y < SizeY; y++) {
		if (y % 2 == 0) {
			for (UINT x = 0; x < SizeX; x++) {
				Index[index++] = y * SizeX + x;
				Index[index++] = (y + 1) * SizeX + x;
			}
		} else {
			for (UINT x = SizeX - 1; x > 0; x--) {
				Index[index++] = (y + 1) * SizeX + x;
				Index[index++] = y * SizeX + x - 1;
			}
		}
	}

	CDynamicVertexBuffer<XMFLOAT3> vb;
	CIndexBuffer ib;

	vb.Initialize(SizeX * (SizeY + 1), Map[0]);
	ib.Initialize(index, Index, true);

	pipeline.SetVertexBuffer(0, &vb);
	pipeline.SetIndexBuffer(&ib, 0);

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
	};
	CInputLayout layout;
	layout.Initialize(inputDesc, array_length(inputDesc), vs);

	pipeline.SetInputLayout(&layout);

	pipeline.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//
	// カメラ設定
	//

	UINT width, height;
	Application::GetScreenSize(width, height);
	CCamera cam(J_PI / 3.0f, (float)width / (float)height, 0.2f, 1000.0f);

	//
	// 定数バッファ設定
	//

	typedef struct {
		XMFLOAT4X4 ViewProjection;
	}cbObj_t;

	CConstantBuffer<cbObj_t> cbObj;
	cbObj.Initialize();
	pipeline.SetVertexShaderConstantBuffer(0, &cbObj);

//	InputMouse::SetRelativeMode(true);

	//
	//　ビューポート
	//
/*	CViewport viewport[2];
	D3D11_VIEWPORT vp;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.Width = (float)width / 2.0f;
	vp.Height = (float)height;
	vp.TopLeftY = 0.0f;
	for (UINT i = 0; i < 2; i++) {
		vp.TopLeftX = (float)(width * i) / 2.0f;
		viewport[i].Initialize(vp);
	}*/

	//
	//　フォント
	//
	CFont font("", 32);

	//
	//　ブレンドステート
	//
	CBlendState blend;
	blend.SetAlignmentBlend();
	blend.ApplyChange();

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));

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

		if (InputMouse::IsPressedRight()) {
			angleYaw += (float)InputMouse::GetRelativeMousePosX() / 500.0f;
			anglePitch += (float)InputMouse::GetRelativeMousePosY() / 500.0f;
		}

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

		static float viewDiff = 0.0f;
		if (InputKeyboard::IsPressed(VK_RIGHT)) {
			viewDiff += 0.005f;
		}
		if (InputKeyboard::IsPressed(VK_LEFT)) {
			viewDiff -= 0.005f;
		}
		XMFLOAT3 eyePosF, targetPosF;
		XMStoreFloat3(&eyePosF, eyePos);
		XMStoreFloat3(&targetPosF, eyePos + frontVec);

		cam.SetCamPos(eyePosF);
		cam.SetTargetPos(targetPosF);
		cam.SetUpVec(XMFLOAT3(0, 1, 0));

		cbObj.UpdateBufferValue({ cam.GetViewProjection() }, Application::GetImmediateContext());

		//
		//　切削
		//

		XMFLOAT3 pos(0, 0, 0);
		auto ret = XMPlaneIntersectLine(
			XMPlaneFromPointNormal(
				XMLoadFloat3(&XMFLOAT3(0, 0, 0)),
				XMLoadFloat3(&XMFLOAT3(0, 1, 0))),
			eyePos, eyePos + frontVec * 10000.0f
		);
		XMStoreFloat3(&pos, ret);
		if (InputMouse::IsPressedLeft()) {
			for (UINT y = 0; y <= SizeY; y++) {
				for (UINT x = 0; x < SizeX; x++) {
					XMFLOAT2 uv;
					Map[y][x].y -= 0.01f 
						/ max(pow((float)y * CellSize - pos.z, 2.0f) + pow((float)x * CellSize - pos.x, 2.0f), 1.0f);
				}
			}
			vb.WriteBuffer(SizeX * (SizeY + 1), Map[0], Application::GetImmediateContext());
			
			pipeline.SetBlendState(&blend);
			Text::DrawString(0, 1, 0.0, 
				std::to_string(pos.x) + ", " 
				+ std::to_string(pos.y) + ", "
				+ std::to_string(pos.z),
				font, pipeline, COLOR(1, 1, 1, 1), 0.5f, 1.0f);
		}

		//
		//　描画
		//

		Application::SetFillMode(D3D11_FILL_WIREFRAME);

		pipeline.DrawIndexed(ib.GetBufferLength());

		Application::SetFillMode(D3D11_FILL_SOLID);

		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}