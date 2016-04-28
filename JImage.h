#pragma once

#include "JShaderResourceView.h"

namespace Javelin {

	class CImage : public CShaderResourceView {
	protected:
		ID3D11ShaderResourceView*	m_pShaderResourceView;
		D3DX11_IMAGE_INFO			m_imageInfo;
		D3DX11_IMAGE_LOAD_INFO		m_imageLoadInfo;
		bool						m_imageLoadInfoUsed;
		std::string					m_filename;
	public:
		CImage() noexcept;
		virtual ~CImage() noexcept;
		CImage(const CImage&);
		CImage& operator=(const CImage&);
		CImage(CImage&&);
		CImage& operator=(CImage&&);

		void Initialize(const std::string& filename, 
			D3DX11_IMAGE_LOAD_INFO* loadInfo = nullptr);
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
	};

}
