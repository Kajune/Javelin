#pragma once

#include "JShaderResourceView.h"

namespace Javelin {

	class CImage : public CShaderResourceView {
	protected:
		ID3D11ShaderResourceView*	m_pShaderResourceView;
		D3DX11_IMAGE_INFO			m_imageInfo;
		D3DX11_IMAGE_LOAD_INFO		m_imageLoadInfo;
		ID3D11Texture2D*			m_texture;
		bool						m_imageLoadInfoUsed;
		bool						m_loadFromTexture;
		std::string					m_filename;
	public:
		CImage() noexcept;
		virtual ~CImage() noexcept;

		void Initialize(const std::string& filename, 
			D3DX11_IMAGE_LOAD_INFO* loadInfo = nullptr);
		void Initialize(ID3D11Texture2D* texture);
		void Cleanup() noexcept;

		ID3D11ShaderResourceView* GetShaderResourceView() const noexcept override {
			return m_pShaderResourceView;
		}
		UINT GetImageWidth() const noexcept {
			return m_imageInfo.Width;
		}
		UINT GetImageHeight() const noexcept {
			return m_imageInfo.Height;
		}
		const D3DX11_IMAGE_INFO& GetImageInfo() const noexcept {
			return m_imageInfo;
		}

		//画像を分割して読み込む
/*		static void LoadDivImage(std::string filename,
								float sizeX, float sizeY,	//分割サイズ
								UINT divX, UINT divY,		//分割数
								UINT begin, UINT end,		//格納を始めるコマ番号と最後に格納するコマ番号
								CImage imageList[],
								D3DX11_IMAGE_LOAD_INFO* loadInfo = nullptr);*/
	};

}
