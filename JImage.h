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

		//�摜�𕪊����ēǂݍ���
/*		static void LoadDivImage(std::string filename,
								float sizeX, float sizeY,	//�����T�C�Y
								UINT divX, UINT divY,		//������
								UINT begin, UINT end,		//�i�[���n�߂�R�}�ԍ��ƍŌ�Ɋi�[����R�}�ԍ�
								CImage imageList[],
								D3DX11_IMAGE_LOAD_INFO* loadInfo = nullptr);*/
	};

}
