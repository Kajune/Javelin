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
		//�t���X�N���[���E�E�B���h�E��؂�ւ���B
		static HRESULT SetIsWindowed(bool isWindowed);
		//�t���X�N���[�����[�h���ǂ����𓾂�B
		static bool GetIsWindowed();
		//�`��̈�̃T�C�Y���擾����B
		static void GetScreenSize(UINT& width, UINT& height) noexcept;
		//���O���o�͂���B
		static void WriteLog(const std::string& text) noexcept;
		//FPS���擾����B
		static float GetFPS() noexcept {
			return m_fps;
		}
		//��ʃT�C�Y���ύX���ꂽ�Ƃ��ɌĂяo���֐����Z�b�g����B
		static void SetCallbackFuncWhenScreensizeChanged(std::function<void(UINT, UINT)> callBack);
		//�X�N���[���V���b�g��ۑ�����
		//���ӁF�}���`�T���v�����O���Ă����Ԃł͉�ʂ̃X�N���[���V���b�g���B�邱�Ƃ��ł��Ȃ�
		static void SaveScreenShot(const std::string& filename, 
			D3DX11_IMAGE_FILE_FORMAT format = D3DX11_IFF_PNG,
			const CTexture2D* texture = nullptr);

		//�������O�ݒ�
		//�V���O���X���b�h�œ��������ǂ��������肷��B�V���O���X���b�h�̏ꍇ�����p�t�H�[�}���X�����シ��B
		//�����l��false
		static void SetIsSingleThreaded(bool isSingleThreaded) noexcept {
			m_isSingleThreaded = isSingleThreaded;
		}
		//�}���`�T���v�����O�@�\�𗘗p���邩�ǂ��������肷��B
		//�����l��true
		static void SetMultiSampleEnabled(bool multiSampleEnabled) noexcept {
			m_multiSampleEnabled = multiSampleEnabled;
		}
		//3D�@�\���g�p���邩�ǂ��������肷��B
		//�����l��true
		static void SetUse3D(bool use3D) noexcept {
			m_use3D = use3D;
		}
		//�A���`�G�C���A�X���g�p���邩�ǂ��������肷��B
		//�����l��false
		static void SetUseAntialias(bool useAntialias) noexcept {
			m_useAntialias = useAntialias;
		}

		//�p�C�v���C���֌W
		//�o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�����ւ���A�w�肳�ꂽ�񐔂�������������ҋ@����B
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
		//�E�B���h�E�n���h�����擾����B
		static HWND GetHWnd() noexcept {
			return m_window.GetHWnd();
		}
		//�x���R���e�L�X�g���擾����B
		static std::shared_ptr<ID3D11DeviceContext> GetDeferredContext();

		//���[�e�B���e�B
		//�T�|�[�g���ꂽ�@�\���x���𓾂�B
		static D3D_FEATURE_LEVEL GetFeatureLevel() noexcept {
			return m_device.GetFeatureLevel();
		}
		//�~�b�v�}�b�v�𐶐�����B
		static void GenerateMips(const CShaderResourceView& srv);
		//�K���}�l��ݒ肷��B
		//�����l��1.0
		static void SetGamma(float value);
		static void SetGamma(const COLOR& value, 
			const COLOR& scale = COLOR(1.0f, 1.0f, 1.0f, 1.0f),
			const COLOR& offset = COLOR(0.0f, 0.0f, 0.0f, 0.0f));
	};
}
