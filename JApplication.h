#pragma once
#include "JGlobal.h"
#include "JViewport.h"
#include "JRasterizerState.h"
#include "JDepthStencilState.h"
#include "JPipeline.h"
#include "JBlendState.h"
#include "JTexture.h"
#include <fstream>
#include <memory>
#include <list>

namespace Javelin {

	class CRenderTarget;
	class CDepthStencil;
	class CCubeTexture;

	class Application final : private UnConstructable {
		Application() = delete;
		~Application() = delete;
		Application(const Application&) = delete;
		Application operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application operator=(Application&&) = delete;

		static class CWindow final {
			HINSTANCE	m_hInst;
			HWND		m_hWnd;
			std::string	m_appName;
			std::string	m_wndClassName;
		public:
			CWindow() noexcept;
			~CWindow() noexcept;

			void Initialize(const std::string& appName, const std::string& wndClassName,
				UINT width, UINT height,
				LRESULT (*WndProc)(HWND, UINT, UINT, LONG),
				HICON icon, HCURSOR cursor);
			void Cleanup() noexcept;

			int MainLoop() noexcept;

			HWND GetHWnd() const noexcept{
				return m_hWnd;
			}
		}m_window;

		static class CDevice final {
			ID3D11Device*			m_pD3D11Device;
			ID3D11DeviceContext*	m_pDeviceContext;
			IDXGISwapChain*			m_pSwapChain;

			D3D_FEATURE_LEVEL		m_supportedFeatureLevel;

			DXGI_SWAP_CHAIN_DESC	m_desc;
		public:
			CDevice() noexcept;
			~CDevice() noexcept;

			void Initialize(
				BOOL isWindowed,
				UINT MSAA_level, 
				DXGI_FORMAT format,
				D3D_DRIVER_TYPE driverType);
			void Cleanup() noexcept;

			void ResizeTarget(UINT width, UINT height);
			void ResizeBuffer(UINT width, UINT height);

			ID3D11Device* GetDevice() const noexcept{
				return m_pD3D11Device;
			}
			ID3D11DeviceContext* GetImmediateContext() const noexcept{
				return m_pDeviceContext;
			}
			IDXGISwapChain* GetSwapChain() const noexcept {
				return m_pSwapChain;
			}
			D3D_FEATURE_LEVEL GetFeatureLevel() const noexcept {
				return m_supportedFeatureLevel;
			}
		}m_device;

		static ID3D11RenderTargetView*	m_pRenderTargetView;
		static ID3D11Texture2D*			m_pDepthStencil;
		static ID3D11DepthStencilView*	m_pDepthStencilView;
		static CDepthStencilState		m_depthStencilState;
		static CRasterizerState			m_rasterizerState;
		static CViewport				m_viewport;
		static CBlendState				m_blendState;
		static std::list<const CPipeline*>	m_pipelineList;

		static bool m_isSingleThreaded;
		static bool m_multiSampleEnabled;
		static bool m_use3D;
		static bool m_useAntialias;

		static std::function<void(UINT, UINT)> m_screenSizeCallback;

		static float m_fps;
		static float m_averageFps;
		static bool m_isWindowSizeChanged;
#if defined(DEBUG) || defined(_DEBUG)
		static std::ofstream m_logFile;
#endif

		static LRESULT MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);
		static void InitDirect3D();
		static void InitBackBuffer();
		static void CleanupDirect3D() noexcept;
		static void CheckDeviceLost();

	public:
		//通常処理関係
		//Javelinの初期化
		static int Initialize(const std::string& appName, UINT width, UINT height,
			bool isWindowed, UINT MSAA_level, 
			HICON icon = nullptr, HCURSOR cursor = nullptr, 
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, 
			D3D_DRIVER_TYPE = D3D_DRIVER_TYPE_HARDWARE) noexcept;
		//メインループ。ループを継続すべきときは0が返ってくる。
		static int MainLoop() noexcept;
		//終了処理。最後に必ず呼び出す必要がある。
		static void Cleanup() noexcept;

		//ユーティリティ
		//ウィンドウのタイトルを変更する。
		static void SetWindowTitle(const std::string& text) noexcept;
		//フルスクリーン・ウィンドウを切り替える。
		static HRESULT SetIsWindowed(bool isWindowed);
		//フルスクリーンモードかどうかを得る。
		static bool GetIsWindowed();
		//描画領域のサイズを取得する。
		static void GetScreenSize(UINT& width, UINT& height) noexcept;
		//ログを出力する。
		static void WriteLog(const std::string& text) noexcept;
		//FPSを取得する。
		static float GetFPS() noexcept {
			return m_fps;
		}
		//画面サイズが変更されたときに呼び出す関数をセットする。
		static void SetCallbackFuncWhenScreensizeChanged(std::function<void(UINT, UINT)> callBack);
		//スクリーンショットを保存する
		//注意：マルチサンプリングしている状態では画面のスクリーンショットを撮ることができない
		static void SaveScreenShot(const std::string& filename, 
			D3DX11_IMAGE_FILE_FORMAT format = D3DX11_IFF_PNG,
			const CTexture2D* texture = nullptr);

		//初期化前設定
		//シングルスレッドで動かすかどうかを決定する。シングルスレッドの場合多少パフォーマンスが向上する。
		//初期値はfalse
		static void SetIsSingleThreaded(bool isSingleThreaded) noexcept {
			m_isSingleThreaded = isSingleThreaded;
		}
		//マルチサンプリング機能を利用するかどうかを決定する。
		//初期値はtrue
		static void SetMultiSampleEnabled(bool multiSampleEnabled) noexcept {
			m_multiSampleEnabled = multiSampleEnabled;
		}
		//3D機能を使用するかどうかを決定する。
		//初期値はtrue
		static void SetUse3D(bool use3D) noexcept {
			m_use3D = use3D;
		}
		//アンチエイリアスを使用するかどうかを決定する。
		//初期値はfalse
		static void SetUseAntialias(bool useAntialias) noexcept {
			m_useAntialias = useAntialias;
		}

		//パイプライン関係
		//バックバッファとフロントバッファを入れ替える、指定された回数だけ垂直同期を待機する。
		static void Present(UINT vSyncInterval = 1);
		static void ClearScreen(const COLOR& color = COLOR(),
			bool clearDepth = true, bool clearStencil = true);
		static void ClearScreen(const CRenderTarget* renderTarget, const CDepthStencil* depthStencil,
			const COLOR& color = COLOR(),
			bool clearDepth = true, bool clearStencil = true);
		static void ClearScreen(const CCubeTexture& cubeTexture,
			const COLOR& color = COLOR(),
			bool clearDepth = true, bool clearStencil = true);
		static void SetDefaultRenderTarget(const CPipeline& pipeline, bool setDepthStencil = true);
		static void SetDefaultDepthStencilState(const CPipeline& pipeline);
		static void SetDefaultRasterizerState(const CPipeline& pipeline);
		static void SetDefaultViewport(const CPipeline& pipeline);
		static void SetDefaultBlendState(const CPipeline& pipeline, 
			const COLOR& blendFactor = COLOR(0.0f, 0.0f, 0.0f, 0.0f), UINT sampleMask = 0xffffff);

		static void AddPipeline(const CPipeline* pipeline);
		static void ErasePipeline(const CPipeline* pipeline);

		static void SetCullMode(D3D11_CULL_MODE mode);
		static void SetFillMode(D3D11_FILL_MODE mode);

		//デバイス関係
		//デバイスを取得する。
		static ID3D11Device* GetDevice() noexcept {
			return m_device.GetDevice();
		}
		//即時デバイスコンテキストを取得する。
		static std::shared_ptr<ID3D11DeviceContext> GetImmediateContext() noexcept;
		//スワップチェインを取得する。
		static IDXGISwapChain* GetSwapChain() noexcept {
			return m_device.GetSwapChain();
		}
		//ウィンドウハンドルを取得する。
		static HWND GetHWnd() noexcept {
			return m_window.GetHWnd();
		}
		//遅延コンテキストを取得する。
		static std::shared_ptr<ID3D11DeviceContext> GetDeferredContext();

		//ユーティリティ
		//サポートされた機能レベルを得る。
		static D3D_FEATURE_LEVEL GetFeatureLevel() noexcept {
			return m_device.GetFeatureLevel();
		}
		//ミップマップを生成する。
		static void GenerateMips(const CShaderResourceView& srv);
		//ガンマ値を設定する。
		//初期値は1.0
		static void SetGamma(float value);
		static void SetGamma(const COLOR& value, 
			const COLOR& scale = COLOR(1.0f, 1.0f, 1.0f, 1.0f),
			const COLOR& offset = COLOR(0.0f, 0.0f, 0.0f, 0.0f));
	};
}
