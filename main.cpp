#include "Javelin.h"

using namespace Javelin;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPInst, LPSTR lpC, int nC) {
	if (Application::Initialize("Javelin", 640, 480, true, 1)){
		Application::Cleanup();
		return -1;
	}

	typedef struct {
		XMFLOAT4 pos;
		XMFLOAT4 col;
	}vertex;

	CDynamicVertexBuffer<vertex> vb;
	vertex pVertices[] = {
		{XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
		{ XMFLOAT4(-0.5f, -0.5f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
	};
	vb.Initialize(3, nullptr);
	vb.WriteBuffer(3, pVertices, Application::GetImmediateContext());
	const CVertexBuffer<vertex>* vbBuffer[] = { &vb };

	UINT strides[] = {
		sizeof(XMFLOAT4) * 2,
	};
	UINT offsets[] = {
		0,
	};

	CIndexBuffer ib;
	UINT index[] = {
		0, 1, 2,
	};
	ib.Initialize(3, index);

	Application::GetDefaultPipeline().SetVertexBuffer(0, 1, vbBuffer, strides, offsets);
	Application::GetDefaultPipeline().SetIndexBuffer(&ib, 0);

	CVertexShader vs;
	vs.Initialize("shader.hlsl", "VS", "vs_4_0");
	CPixelShader ps;
	ps.Initialize("shader.hlsl", "PS", "ps_4_0");

	Application::GetDefaultPipeline().SetVertexShader(&vs);
	Application::GetDefaultPipeline().SetPixelShader(&ps);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	CInputLayout layout;
	layout.Initialize(inputDesc, 2, vs);

	Application::GetDefaultPipeline().SetInputLayout(&layout);
	Application::GetDefaultPipeline().SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.0f, 1.0f, 1.0f));
		pVertices[0].pos.y = sinf(float(timeGetTime()) * J_PI / 1000.0f);
		vb.WriteBuffer(3, pVertices, Application::GetImmediateContext());
		Application::GetDefaultPipeline().DrawIndexed(3);
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
		Application::Present();
	}
	Application::Cleanup();
	return 0;
}
