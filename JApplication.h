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
		//�ʏ폈���֌W
		//Javelin�̏�����
		static int Initialize(const std::string& appName, UINT width, UINT height,
			bool isWindowed, UINT MSAA_level, 
			HICON icon = nullptr, HCURSOR cursor = nullptr, 
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, 
			D3D_DRIVER_TYPE = D3D_DRIVER_TYPE_HARDWARE) noexcept;
		//���C�����[�v�B���[�v���p�����ׂ��Ƃ���0���Ԃ��Ă���B
		static int MainLoop() noexcept;
		//�I�������B�Ō�ɕK���Ăяo���K�v������B
		static void Cleanup() noexcept;

		//���[�e�B���e�B
		//�E�B���h�E�̃^�C�g����ύX����B
		static void SetWindowTitle(const std::string& text) noexcept;
		//�`��̈�̃T�C�Y���擾����B
		static void GetScreenSize(UINT& width, UINT& height) noexcept;
		//���O���o�͂���B
		static void WriteLog(const std::string& text) noexcept;
		//FPS���擾����B
		static float GetFPS() noexcept {
			return m_fps;
		}

		//�������O�ݒ�
		//�V���O���X���b�h�œ��������ǂ��������肷��B�V���O���X���b�h�̏ꍇ�����p�t�H�[�}���X�����シ��B
		//�����l��false
		static void SetIsSingleThreaded(bool isSingleThreaded) noexcept {
			m_isSingleThreaded = isSingleThreaded;
		}
		//3D�@�\���g�p���邩�ǂ��������肷��B
		//�����l��true
		static void SetUse3D(bool use3D) noexcept {
			m_use3D = use3D;
		}

		//�p�C�v���C���֌W
		//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�����ւ���A�w�肳�ꂽ�񐔂�������������ҋ@����B
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

		//�f�o�C�X�֌W
		//�f�o�C�X���擾����B
		static ID3D11Device* GetDevice() noexcept {
			return m_device.GetDevice();
		}
		//�����f�o�C�X�R���e�L�X�g���擾����B
		static std::shared_ptr<ID3D11DeviceContext> GetImmediateContext() noexcept;
		//�X���b�v�`�F�C�����擾����B
		static IDXGISwapChain* GetSwapChain() noexcept {
			return m_device.GetSwapChain();
		}
		//�x���R���e�L�X�g���擾����B
		static std::shared_ptr<ID3D11DeviceContext> GetDeferredContext();
	};

}
