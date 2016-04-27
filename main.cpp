#include "Javelin.h"

using namespace Javelin;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR lpC, int nC) {
	if (Application::Initialize("Javelin", 640, 480, true, 1)){
		Application::Cleanup();
		return -1;
	}

	typedef struct {
		XMFLOAT3 pos;
		XMFLOAT3 col;
	}vertex;

	CVertexBuffer<vertex> vb;
	vertex pVertices[] = {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
	};
	vb.Initialize(array_length(pVertices), pVertices);

	CIndexBuffer ib;
	UINT index[] = {
		0, 1, 3,	1, 2, 3,	1, 5, 2,	5, 6, 2,	5, 4, 6,	4, 7, 6,
		4, 5, 0,	5, 1, 0,	4, 0, 7,	0, 3, 7,	3, 2, 7,	2, 6, 7,
	};
	ib.Initialize(array_length(index), index);

	Application::GetDefaultPipeline().SetVertexBuffer(0, &vb);
	Application::GetDefaultPipeline().SetIndexBuffer(&ib, 0);

	CVertexShader vs;
	vs.Initialize("shader.hlsl", "VS", "vs_4_0");
	CGeometryShader gs;
	gs.Initialize("shader.hlsl", "GS", "gs_4_0");
	CPixelShader ps;
	ps.Initialize("shader.hlsl", "PS", "ps_4_0");

	Application::GetDefaultPipeline().SetVertexShader(&vs);
	Application::GetDefaultPipeline().SetGeometryShader(&gs);
	Application::GetDefaultPipeline().SetPixelShader(&ps);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	CInputLayout layout;
	layout.Initialize(inputDesc, 2, vs);

	Application::GetDefaultPipeline().SetInputLayout(&layout);
	Application::GetDefaultPipeline().SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT width, height;
	Application::GetScreenSize(width, height);
	CCamera cam(J_PI / 6.0f, (float)width / (float)height, 1.0f, 20.0f);

	typedef struct {
		XMFLOAT4X4 view;
		XMFLOAT3 light;
		FLOAT dummy;
	}cbChangesEveryFrame_t;

	CConstantBuffer<XMFLOAT4X4> cbNeverChanges;
	CConstantBuffer<cbChangesEveryFrame_t> cbChangesEveryFrame;
	CConstantBuffer<XMFLOAT4X4> cbChangesEveryObject;

	cbNeverChanges.Initialize();
	cbChangesEveryFrame.Initialize();
	cbChangesEveryObject.Initialize();

	Application::GetDefaultPipeline().SetVertexShaderConstantBuffer(0, &cbNeverChanges);
	Application::GetDefaultPipeline().SetGeometryShaderConstantBuffer(0, &cbNeverChanges);
	Application::GetDefaultPipeline().SetPixelShaderConstantBuffer(0, &cbNeverChanges);

	Application::GetDefaultPipeline().SetVertexShaderConstantBuffer(1, &cbChangesEveryFrame);
	Application::GetDefaultPipeline().SetGeometryShaderConstantBuffer(1, &cbChangesEveryFrame);
	Application::GetDefaultPipeline().SetPixelShaderConstantBuffer(1, &cbChangesEveryFrame);

	Application::GetDefaultPipeline().SetVertexShaderConstantBuffer(2, &cbChangesEveryObject);
	Application::GetDefaultPipeline().SetGeometryShaderConstantBuffer(2, &cbChangesEveryObject);
	Application::GetDefaultPipeline().SetPixelShaderConstantBuffer(2, &cbChangesEveryObject);

	cbNeverChanges.UpdateBufferValue(cam.GetProjection(), Application::GetImmediateContext());

	Application::SetCullMode(D3D11_CULL_NONE);
	Application::SetDefaultRasterizerState();

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.125f, 0.3f, 1.0f));
		
		cam.SetCamPos(XMFLOAT3(0, 5, -5));
		cam.SetTargetPos(XMFLOAT3(0, 0, 0));
		cbChangesEveryFrame.UpdateBufferValue(cbChangesEveryFrame_t{ cam.GetView(), XMFLOAT3(1, 1, -2) }, 
			Application::GetImmediateContext());

		XMMATRIX matX, matY;
		float rotate = (float)(J_PI * (timeGetTime() % 1500)) / 1500.0f;
		matY = XMMatrixRotationY(rotate);
		rotate = (float)(J_PI * (timeGetTime() % 1500)) / 750.0f;
		matX = XMMatrixRotationX(rotate);
		XMFLOAT4X4 world;
		XMStoreFloat4x4(&world, XMMatrixTranspose(matY * matX));
		cbChangesEveryObject.UpdateBufferValue(world, Application::GetImmediateContext());
		
		Application::GetDefaultPipeline().DrawIndexed(array_length(index));
		
		Application::Present();
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
	}
	Application::Cleanup();
	return 0;
}
