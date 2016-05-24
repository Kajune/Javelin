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
	// ���b�V���f�[�^
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
	CMesh<XMFLOAT3, alloc> mesh_shadow;
	mesh_shadow.Initialize(loader);

	//
	// �V�F�[�_
	//

	CVertexShader vs, vs_shadow;
	CPixelShader ps;

	vs.Initialize("shader.hlsl", "VS", "vs_4_0");
	vs_shadow.Initialize("shader.hlsl", "VS_Shadow", "vs_4_0");
	ps.Initialize("shader.hlsl", "PS", "ps_4_0");

	//
	// ���̓��C�A�E�g
	//

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
	//�@�[�x�o�b�t�@
	//

	constexpr int smSize = 2048;
	CDepthStencil dsShadow;
	dsShadow.Initialize(smSize, smSize);

	//
	//�@�r���[�|�[�g
	//

	CViewport vpShadow;
	vpShadow.Initialize(smSize, smSize);

	//
	// �J�����ݒ�
	//

	UINT width, height;
	Application::GetScreenSize(width, height);
	CCamera cam(J_PI / 3.0f, (float)width / (float)height, 0.2f, 1000.0f);
	CCamera camShadow(J_PI / 6.0f, 1.0f, 0.2f, 1000.0f);

	//
	// �T���v���ݒ�
	//

	CSamplerState sampler;
	sampler.Initialize(D3D11_TEXTURE_ADDRESS_BORDER,
		D3D11_FILTER_ANISOTROPIC, COLOR(1, 1, 1, 1));
	pipeline.SetPixelShaderSamplerState(0, &sampler);

	//
	//�@�f�t�H���g�e�N�X�`��
	//

	CImage defaultWhite;
	defaultWhite.Initialize("resource/white.png");

	//
	// �萔�o�b�t�@�ݒ�
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

	XMFLOAT3 LightPos(10.0f, 10.0f, -10.0f);

	cbLight_t cbLightValue;
	cbLightValue.numLights = 1;
	cbLightValue.lightPos[0] = XMFLOAT4(LightPos.x, LightPos.y, LightPos.z, 0.0f);
	cbLightValue.specularLight[0] = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	cbLightValue.diffuseLight[0] = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
	cbLightValue.ambientLight = COLOR(0.5f, 0.5f, 0.5f, 1.0f).ary.rgba;
	cbLightValue.lightPower = 50.0f;

	InputMouse::SetRelativeMode(true);

	//
	//�@�T�E���h
	//

	CSound sound;
	sound.Initialize("resource/heli.wav");

	CSound3D::Initialize(1.0f);

	CSound3D sound3D;
	SoundPlayer::PlayLoop(sound, -1, nullptr, &sound3D);

	//
	//�@�r���[�|�[�g
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

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));

		//
		//�@�I�u�W�F�N�g�z�u
		//
		cbObj_t cbObjValue;
		auto mat = XMMatrixTranspose(
			XMMatrixScaling(0.02f, 0.02f, 0.02f)
			* XMMatrixRotationY(timeGetTime() / 5000.0f));
		XMStoreFloat4x4(&cbObjValue.World, mat);

		cbObj.UpdateBufferValue(cbObjValue, Application::GetImmediateContext());

		//
		// �J�����z�u
		//
		static float angleYaw = 0.0f, anglePitch = 0.0f;
		XMVECTOR frontVec = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
		frontVec = XMVector3Transform(frontVec, XMMatrixRotationRollPitchYaw(anglePitch, angleYaw, 0.0f));
		XMVECTOR rightVec = XMLoadFloat3(&XMFLOAT3(1, 0, 0));
		rightVec = XMVector3Transform(rightVec, XMMatrixRotationRollPitchYaw(anglePitch, angleYaw, 0.0f));
		XMVECTOR upVec = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
		upVec = XMVector3Transform(upVec, XMMatrixRotationRollPitchYaw(anglePitch, angleYaw, 0.0f));

		angleYaw += InputMouse::GetRelativeMousePosX() / 500.0f;
		anglePitch += InputMouse::GetRelativeMousePosY() / 500.0f;

		static XMVECTOR eyePos;

		constexpr float speed = 0.1f;
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

		static float viewDiff = 0.0f;
		if (InputKeyboard::IsPressed(VK_RIGHT)) {
			viewDiff += 0.005f;
		}
		if (InputKeyboard::IsPressed(VK_LEFT)) {
			viewDiff -= 0.005f;
		}

		for (UINT i = 0; i < 2; i++) {
			Application::ClearScreen(nullptr, &dsShadow, COLOR(1.0f, 1.0f, 1.0f, 1.0f));

			XMFLOAT3 eyePosF, targetPosF;
			XMStoreFloat3(&eyePosF, eyePos - rightVec * ((float)i - 0.5f) * viewDiff);
			XMStoreFloat3(&targetPosF, eyePos + frontVec);

			cam.SetCamPos(eyePosF);
			cam.SetTargetPos(targetPosF);
			cam.SetUpVec(XMFLOAT3(0, 1, 0));
			cbObjValue.ViewProjection = cam.GetViewProjection();

			camShadow.SetCamPos(LightPos);
			camShadow.SetTargetPos(targetPosF);
			cbObjValue.ViewProjectionShadow = camShadow.GetViewProjection();
			cbObj.UpdateBufferValue(cbObjValue, Application::GetImmediateContext());

			cbLightValue.eyePos = eyePosF;
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
			pipeline.SetViewports(viewport[i]);

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
		}

		sound3D.SetListenerParam(eyePos, frontVec, XMLoadFloat3(&XMFLOAT3(0, 1, 0)), actualSpeed);
		sound3D.SetEmitterParam(XMLoadFloat3(&XMFLOAT3(0, 0, 0)), XMLoadFloat3(&XMFLOAT3(0, 0, 1)));
		CSound3D::applyVoiceAll();

		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}