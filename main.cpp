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
		{XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
		{ XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
	};
	vb.Initialize(3, nullptr);
	vb.WriteBuffer(3, pVertices, Application::GetImmediateContext());
	const CVertexBuffer<vertex>* vbBuffer[] = { &vb };

	UINT strides[] = {
		sizeof(XMFLOAT4), sizeof(XMFLOAT4),
	};
	UINT offsets[] = {
		0, 0,
	};

	CIndexBuffer ib;
	UINT index[] = {
		0, 1, 2,
	};
	ib.Initialize(3, index);

	Application::GetDefaultPipeline().SetVertexBuffer(0, 1, vbBuffer, strides, offsets);
	Application::GetDefaultPipeline().SetIndexBuffer(ib, 0);

	while (Application::MainLoop() == 0) {
		Application::ClearScreen(COLOR(0.0f, 0.0f, 1.0f, 1.0f));
		Application::SetWindowTitle(std::to_string(Javelin::Application::GetFPS()));
		Application::Present();
	}
	Application::Cleanup();
	return 0;
}
