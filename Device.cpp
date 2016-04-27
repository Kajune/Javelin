#include "JApplication.h"
#include "JUtility.h"

using namespace Javelin;

Application::CDevice::CDevice() noexcept {
}

Application::CDevice::~CDevice() noexcept {
}

void Application::CDevice::Initialize(
	BOOL isWindowed,
	UINT MSAA_level,
	DXGI_FORMAT format, 
	D3D_DRIVER_TYPE driverType) {
	D3D_FEATURE_LEVEL FeatuerLevels[] = {
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	D3D_FEATURE_LEVEL pFeatureLevel[array_length(FeatuerLevels)];

	UINT flag = m_isSingleThreaded ? D3D11_CREATE_DEVICE_SINGLETHREADED : 0;
#if defined(DEBUG) || defined(_DEBUG)
	flag = flag | D3D11_CREATE_DEVICE_DEBUG;
#endif

	WriteLog("デバイスの作成");
	if (FAILED(D3D11CreateDevice(nullptr, driverType, nullptr, flag, 
		FeatuerLevels, array_length(FeatuerLevels), 
		D3D11_SDK_VERSION, &m_pD3D11Device, pFeatureLevel, &m_pDeviceContext))) {
		WriteLog("デバイスの作成に失敗しました");
		throw -1;
	}

	WriteLog("フォーマット対応状況の確認");
	UINT support;
	m_pD3D11Device->CheckFormatSupport(format, &support);
	if (!(support & D3D11_FORMAT_SUPPORT_RENDER_TARGET)) {
		WriteLog("指定されたフォーマットはレンダーターゲットとしてサポートされていません");
		throw -1;
	}

	UINT width, height;
	GetScreenSize(width, height);

	m_desc.BufferCount = 3;
	m_desc.BufferDesc.Width = width;
	m_desc.BufferDesc.Height = height;
	m_desc.BufferDesc.Format = format;
	m_desc.BufferDesc.RefreshRate.Numerator = 60;
	m_desc.BufferDesc.RefreshRate.Denominator = 1;
	m_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	m_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	m_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	m_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_desc.OutputWindow = m_window.GetHWnd();
	m_desc.Windowed = isWindowed;
	m_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	WriteLog("MSAAの設定");

	DXGI_SAMPLE_DESC MSAA;
	MSAA.Count = 1;
	MSAA.Quality = 0;
	if (MSAA_level > 1) {
		for (UINT i = 0; i <= MSAA_level && i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i++) {
			UINT sampleQuality;
			if (SUCCEEDED(m_pD3D11Device->CheckMultisampleQualityLevels(format, i, &sampleQuality))) {
				if (0 < sampleQuality) {
					MSAA.Count = i;
					MSAA.Quality = sampleQuality - 1;
				}
			}
		}
	}

	m_desc.SampleDesc = MSAA;
	WriteLog("MSAA: Count = " + std::to_string(MSAA.Count) + ", Quality = " + std::to_string(MSAA.Quality));
	
	IDXGIDevice* pIDXGIDevice;
	if (FAILED(m_pD3D11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pIDXGIDevice))) {
		WriteLog("IDXGIDeviceの取得に失敗しました");
		throw - 1;
	}

	IDXGIAdapter* pIDXGIAdapter;
	if (FAILED(pIDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pIDXGIAdapter))) {
		SAFE_RELEASE(pIDXGIDevice);
		WriteLog("IDXGIAdapterの取得に失敗しました");
		throw - 1;
	}

	IDXGIFactory* pIDXGIFactory;
	if (FAILED(pIDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pIDXGIFactory))) {
		SAFE_RELEASE(pIDXGIAdapter);
		SAFE_RELEASE(pIDXGIDevice);
		WriteLog("IDXGIFactoryの取得に失敗しました");
		throw - 1;
	}

	WriteLog("スワップチェインの作成");
	HRESULT hr = pIDXGIFactory->CreateSwapChain(m_pD3D11Device, &m_desc, &m_pSwapChain);
	pIDXGIFactory->MakeWindowAssociation(m_window.GetHWnd(), 0);
	SAFE_RELEASE(pIDXGIFactory);
	SAFE_RELEASE(pIDXGIAdapter);
	SAFE_RELEASE(pIDXGIDevice);
	if (FAILED(hr)) {
		WriteLog("スワップチェインの作成に失敗しました");
		throw - 1;
	}
}

void Application::CDevice::Cleanup() noexcept {
	if (m_pSwapChain) {
		m_pSwapChain->SetFullscreenState(FALSE, nullptr);
	}
	if (m_pDeviceContext) {
		m_pDeviceContext->ClearState();
	}
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pD3D11Device);
}

void Application::CDevice::ResizeTarget(UINT width, UINT height) {
	m_desc.BufferDesc.Width = width;
	m_desc.BufferDesc.Height = height;
	if (FAILED(m_pSwapChain->ResizeTarget(&m_desc.BufferDesc))) {
		throw - 1;
	}
}
