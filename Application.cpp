#include "JApplication.h"
#include <locale.h>
#include <time.h>
#include "JUtility.h"

using namespace Javelin;

ID3D11RenderTargetView*	Application::m_pRenderTargetView = nullptr;
CDepthStencil Application::m_depthStencil;
CDepthStencilState Application::m_depthStencilState;
CRasterizerState Application::m_rasterizerState;
CViewport Application::m_viewport;
CBlendState Application::m_blendState;
CPipeline Application::m_pipeline;

Application::CWindow Application::m_window;
Application::CDevice Application::m_device;
bool Application::m_isSingleThreaded = false;
bool Application::m_isWindowSizeChanged = true;
bool Application::m_use3D = true;
bool Application::m_useAntialias = false;
float Application::m_fps = 0.0f;
float Application::m_averageFps = 0.0f;
#if defined(DEBUG) || defined(_DEBUG)
std::ofstream Application::m_logFile("log.txt");
#endif

//
//　通常処理関係
//

int Application::Initialize(const std::string& appName, UINT width, UINT height,
	bool isWindowed, UINT MSAA_level, HICON icon, HCURSOR cursor,
	DXGI_FORMAT format, D3D_DRIVER_TYPE driverType) noexcept{
	WriteLog("アプリケーションを開始します");
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	setlocale(LC_ALL, "jpn");

	if (!XMVerifyCPUSupport()) {
		WriteLog("XNA Mathがサポートされていません");
		return -1;
	}
	
	try {
		WriteLog("ウィンドウの初期化");
		m_window.Initialize(appName, appName, width, height, MainWndProc, icon, cursor);
	} catch (...) {
		WriteLog("ウィンドウの初期化に失敗しました");
		return -1;
	}

	try {
		WriteLog("DirectXドライバの初期化");
		m_device.Initialize(isWindowed, MSAA_level, format, driverType);
	} catch (...) {
		WriteLog("DirectXドライバの初期化に失敗しました");
		return -1;
	}

	try {
		WriteLog("Direct3Dの初期化");
		InitDirect3D();
	} catch (...) {
		WriteLog("Direct3Dの初期化に失敗しました");
		return -1;
	}

	try {
		WriteLog("バックバッファの初期化");
		InitBackBuffer();
	} catch (...) {
		WriteLog("バックバッファの初期化に失敗しました");
		return -1;
	}

	WriteLog("アプリケーションの初期化が完了しました\n");

	return 0;
}

int Application::MainLoop() noexcept {
	//デバイスチェック
	try {
		CheckDeviceLost();
	} catch (...) {
		WriteLog("デバイスに問題が発生したため、アプリケーションの実行を中断します");
		return -1;
	}

	//FPS計測
	static int count = 1;
	static int sampleCount = 0;
	static int fpsTime = timeGetTime();
	if (count % 60 == 0) {
		m_fps = 60 * 1000.0f / (float)(timeGetTime() - fpsTime);
		fpsTime = timeGetTime();
		m_averageFps = (m_averageFps * sampleCount + m_fps) / ++sampleCount;
	}
	count++;
	return m_window.MainLoop();
}

void Application::Cleanup() noexcept {
	WriteLog("平均FPSは" + std::to_string(m_averageFps) + "でした");
	m_blendState.Cleanup();
	m_depthStencilState.Cleanup();
	m_rasterizerState.Cleanup();
	CleanupDirect3D();
	WriteLog("Direct3D関係の解放が完了しました");
	m_device.Cleanup();
	WriteLog("デバイスの解放が完了しました");
	m_window.Cleanup();
	WriteLog("ウィンドウの解放が完了しました");
	CoUninitialize();
	WriteLog("アプリケーションを終了します");
}

//
//ユーティリティ
//

void Application::SetWindowTitle(const std::string& text) noexcept {
	SetWindowText(m_window.GetHWnd(), text.c_str());
}

void Application::GetScreenSize(UINT& width, UINT& height) noexcept {
	static UINT width_t = 0;
	static UINT height_t = 0;
	if (m_isWindowSizeChanged) {
		RECT rc;
		GetClientRect(m_window.GetHWnd(), &rc);
		width_t = rc.right - rc.left;
		height_t = rc.bottom - rc.top;
		m_isWindowSizeChanged = false;
	}
	width = width_t;
	height = height_t;
}

void Application::WriteLog(const std::string& text) noexcept {
#if defined(DEBUG) || defined(_DEBUG)
	m_logFile << clock() << ":" << text << std::endl;
	OutputDebugString((text + "\n").c_str());
#endif
}

//
//　初期化前設定
//

//
//　パイプライン関係
//

HRESULT Application::Present(UINT vSyncInterval) {
	if (!GetSwapChain()) {
		WriteLog("スワップチェインの原因不明な消失");
		return E_FAIL;
	}

	return GetSwapChain()->Present(vSyncInterval, 0);
}

void Application::ClearScreen(const COLOR& color, bool clearDepth, bool clearStencil) {
	m_device.GetImmediateContext()->ClearRenderTargetView(m_pRenderTargetView, color.ary.rgba);
	if (m_use3D) {
		UINT clearFlag = (clearDepth ? D3D11_CLEAR_DEPTH : 0) | (clearStencil ? D3D11_CLEAR_STENCIL : 0);
		m_device.GetImmediateContext()->ClearDepthStencilView(m_depthStencil.GetDepthStencilView(),
			clearFlag, 1.0f, 0);
	}
}

void Application::SetDefaultRenderTarget(bool setDepthStencil) {
	SetDefaultRenderTarget(m_pipeline, setDepthStencil);
}

void Application::SetDefaultRenderTarget(const CPipeline& pipeline, bool setDepthStencil) {
	pipeline.SetRenderTarget(1, &m_pRenderTargetView, setDepthStencil ? &m_depthStencil : nullptr);
}

void Application::SetDefaultDepthStencilState() {
	SetDefaultDepthStencilState(m_pipeline);
}

void Application::SetDefaultDepthStencilState(const CPipeline& pipeline) {
	pipeline.SetDepthStencilState(&m_depthStencilState, 0);
}

void Application::SetDefaultRasterizerState() {
	SetDefaultRasterizerState(m_pipeline);
}

void Application::SetDefaultRasterizerState(const CPipeline& pipeline) {
	pipeline.SetRasterizerState(&m_rasterizerState);
}

void Application::SetDefaultViewport() {
	SetDefaultViewport(m_pipeline);
}

void Application::SetDefaultViewport(const CPipeline& pipeline) {
	const CViewport* vp[] = { &m_viewport };
	pipeline.SetViewports(1, vp);
}

void Application::SetDefaultBlendState(const COLOR& blendFactor, UINT sampleMask) {
	SetDefaultBlendState(m_pipeline, blendFactor, sampleMask);
}

void Application::SetDefaultBlendState(const CPipeline& pipeline, const COLOR& blendFactor, UINT sampleMask) {
	pipeline.SetBlendState(&m_blendState, blendFactor, sampleMask);
}

void Application::SetCullMode(D3D11_CULL_MODE mode) {
	m_rasterizerState.GetRasterizerDesc().CullMode = mode;
	m_rasterizerState.ApplyChange();
}

//
//　デバイス関係
//

std::shared_ptr<ID3D11DeviceContext> Application::GetImmediateContext() noexcept {
	return std::shared_ptr<ID3D11DeviceContext>(m_device.GetImmediateContext(),
		[](ID3D11DeviceContext* p) {});
}


std::shared_ptr<ID3D11DeviceContext> Application::GetDeferredContext() {
	if (!GetDevice()) {
		WriteLog("デバイスの原因不明な消失");
		throw - 1;
	}

	ID3D11DeviceContext* pDeviceContext;
	if (FAILED(GetDevice()->CreateDeferredContext(0, &pDeviceContext))) {
		WriteLog("遅延コンテキストの作成失敗");
		throw - 1;
	}

	return std::shared_ptr<ID3D11DeviceContext>(pDeviceContext,
		[](ID3D11DeviceContext* p) {SAFE_RELEASE(p); });
}

//
//　以下、内部実装
//

void Application::InitDirect3D() {
	WriteLog("ラスタライザステートの作成");
	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.DepthClipEnable = m_use3D;
	rsDesc.ScissorEnable = false;
	rsDesc.MultisampleEnable = false;
	rsDesc.AntialiasedLineEnable = m_useAntialias;
	m_rasterizerState.Initialize(rsDesc);

	WriteLog("デプスステンシルステートの作成");
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = m_use3D;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	m_depthStencilState.Initialize(dsDesc);
}

void Application::InitBackBuffer() {
	WriteLog("レンダーターゲットビューの作成");
	ID3D11Texture2D* pBackBuffer;
	if (FAILED(GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) {
		WriteLog("バックバッファの取得に失敗しました");
		throw - 1;
	}

	D3D11_TEXTURE2D_DESC descBackBuffer;
	pBackBuffer->GetDesc(&descBackBuffer);

	SAFE_RELEASE(m_pRenderTargetView);
	HRESULT hr = GetDevice()->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);
	if (FAILED(hr)) {
		WriteLog("レンダーターゲットビューの作成に失敗しました");
		throw - 1;
	}

	if (m_use3D) {
		WriteLog("デプスステンシルビューの作成");
		m_depthStencil.Initialize(descBackBuffer.Width, descBackBuffer.Height);
	}

	WriteLog("ビューポートの設定");
	m_viewport.Initialize(descBackBuffer.Width, descBackBuffer.Height);

	WriteLog("パイプラインの設定");
	m_pipeline.Initialize(GetImmediateContext());

	SetDefaultRenderTarget(m_use3D);
	SetDefaultDepthStencilState();
	SetDefaultRasterizerState();
	SetDefaultViewport();
	SetDefaultBlendState();
}


void Application::CheckDeviceLost() {
	if (!GetDevice()) {
		WriteLog("原因不明なデバイスの消失");
		throw - 1;
	}
	HRESULT hr = GetDevice()->GetDeviceRemovedReason();
	if (FAILED(hr)) {
		switch (hr) {
			case DXGI_ERROR_DEVICE_HUNG:
				WriteLog("正しくないコマンドによりデバイスが失敗しました。これは設計時の問題です");
				throw - 1;
			case DXGI_ERROR_DEVICE_REMOVED:
				WriteLog("ビデオカードが接続されていません");
				throw - 1;
			case DXGI_ERROR_DEVICE_RESET:
				WriteLog("正しくないコマンドによりデバイスが失敗しました。これは実行時の問題です");
				throw - 1;
			case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
				WriteLog("ドライバ内で問題が発生しました");
				throw - 1;
			case E_OUTOFMEMORY:
				WriteLog("メモリ不足のため動作を停止しました");
				throw - 1;
			default:
				WriteLog("原因不明なデバイスの消失");
				throw - 1;
		}
	}
}

void Application::CleanupDirect3D() noexcept {
	m_depthStencil.Cleanup();
	SAFE_RELEASE(m_pRenderTargetView);
}

LRESULT Application::MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam) {
	switch (msg) {
		case WM_SIZE:
			WriteLog("ウィンドウサイズの変更");
			if (!GetDevice() || !m_device.GetImmediateContext() || !GetSwapChain() || wParam == SIZE_MINIMIZED) {
				break;
			}

			CleanupDirect3D();

			try {
				m_device.ResizeTarget(LOWORD(lParam), HIWORD(lParam));
			} catch (...) {
				WriteLog("ウィンドウサイズの変更に失敗しました");
			}

			try {
				InitBackBuffer();
			} catch (...) {
				WriteLog("バックバッファのサイズ変更に失敗しました");
			}

			m_isWindowSizeChanged = true;
			break;
		case WM_DESTROY:
			WriteLog("");
			WriteLog("破棄命令が出されました");
			PostQuitMessage(0);
			break;
		default:
			break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

