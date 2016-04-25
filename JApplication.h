#pragma once
#include "JGlobal.h"
#include "JDepthStencil.h"
#include "JViewport.h"
#include "JRasterizerState.h"
#include "JDepthStencilState.h"
#include "JPipeline.h"
#include <fstream>
#include <memory>

namespace Javelin {

	class Application final {
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

			ID3D11Device* GetDevice() const noexcept{
				return m_pD3D11Device;
			}
			ID3D11DeviceContext* GetImmediateContext() const noexcept{
				return m_pDeviceContext;
			}
			IDXGISwapChain* GetSwapChain() const noexcept {
				return m_pSwapChain;
			}
		}m_device;

		static ID3D11RenderTargetView*	m_pRenderTargetView;
		static CDepthStencil			m_depthStencil;
		static CDepthStencilState		m_depthStencilState;
		static CRasterizerState			m_rasterizerState;
		static CViewport				m_viewport;
		static CPipeline				m_pipeline;

		static bool m_isSingleThreaded;
		static bool m_use3D;
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
		//描画領域のサイズを取得する。
		static void GetScreenSize(UINT& width, UINT& height) noexcept;
		//ログを出力する。
		static void WriteLog(const std::string& text) noexcept;
		//FPSを取得する。
		static float GetFPS() noexcept {
			return m_fps;
		}

		//初期化前設定
		//シングルスレッドで動かすかどうかを決定する。シングルスレッドの場合多少パフォーマンスが向上する。
		//初期値はfalse
		static void SetIsSingleThreaded(bool isSingleThreaded) noexcept {
			m_isSingleThreaded = isSingleThreaded;
		}
		//3D機能を使用するかどうかを決定する。
		//初期値はtrue
		static void SetUse3D(bool use3D) noexcept {
			m_use3D = use3D;
		}

		//パイプライン関係
		//バックバッファとフロントバッファを入れ替える、指定された回数だけ垂直同期を待機する。
		static HRESULT Present(UINT vSyncInterval = 1);
		static void ClearScreen(const COLOR& color = COLOR(),
			bool clearDepth = true, bool clearStencil = true);
		static void SetDefaultRenderTarget(bool setDepthStencil);
		static void SetDefaultRenderTarget(const CPipeline& pipeline, bool setDepthStencil);
		static void SetDefaultDepthStencilState();
		static void SetDefaultDepthStencilState(const CPipeline& pipeline);
		static void SetDefaultRasterizerState();
		static void SetDefaultRasterizerState(const CPipeline& pipeline);
		static void SetDefaultViewport();
		static void SetDefaultViewport(const CPipeline& pipeline);
		static const CPipeline& GetDefaultPipeline() {
			return m_pipeline;
		}

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
		//遅延コンテキストを取得する。
		static std::shared_ptr<ID3D11DeviceContext> GetDeferredContext();
	};

}
