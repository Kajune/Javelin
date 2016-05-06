#pragma once

#include "JTexture.h"
#include "JApplication.h"
#include "JUtility.h"

namespace Javelin {

	template <typename TextureType>
	CTexture<TextureType>::CTexture() noexcept :
	m_pTexture(nullptr),
		m_pShaderResourceView(nullptr) {
	}

	template <typename TextureType>
	CTexture<TextureType>::~CTexture() noexcept {
		Cleanup();
	}

	/*
	template <typename TextureType>
	CTexture<TextureType>::CTexture(const CTexture<TextureType>& texture) {
		operator=(texture);
	}

	template <typename TextureType>
	CTexture<TextureType>& CTexture<TextureType>::operator=(const CTexture<TextureType>& texture) {
		if (this == &texture) {
			return *this;
		}
		Cleanup();
		Initialize(texture.m_desc, &texture.m_srvDesc, nullptr);
		Application::GetImmediateContext()->CopyResource(m_pTexture, texture.m_pTexture);
		return *this;
	}

	template <typename TextureType>
	CTexture<TextureType>::CTexture(CTexture<TextureType>&& texture) {
		operator=(std::move(texture));
	}

	template <typename TextureType>
	CTexture<TextureType>& CTexture<TextureType>::operator=(CTexture<TextureType>&& texture) {
		if (this == &texture) {
			return *this;
		}
		Cleanup();
		m_pTexture = texture.m_pTexture;
		m_pShaderResourceView = texture.m_pShaderResourceView;
		m_desc = texture.m_desc;
		m_srvDesc = texture.m_srvDesc;
		return *this;
	}*/

	template<>
	HRESULT CTexture<ID3D11Texture1D>::CreateTexture(const DescType& desc,
		const D3D11_SUBRESOURCE_DATA* subResource) {
		return Application::GetDevice()->CreateTexture1D(&desc, subResource, &m_pTexture);
	}

	template<>
	HRESULT CTexture<ID3D11Texture2D>::CreateTexture(const DescType& desc,
		const D3D11_SUBRESOURCE_DATA* subResource) {
		return Application::GetDevice()->CreateTexture2D(&desc, subResource, &m_pTexture);
	}

	template<>
	HRESULT CTexture<ID3D11Texture3D>::CreateTexture(const DescType& desc,
		const D3D11_SUBRESOURCE_DATA* subResource) {
		return Application::GetDevice()->CreateTexture3D(&desc, subResource, &m_pTexture);
	}

	template <typename TextureType>
	struct GetViewDimension {
		static const D3D11_SRV_DIMENSION value;
	};

	template <>
	struct GetViewDimension <ID3D11Texture1D> {
		static const D3D11_SRV_DIMENSION value = D3D11_SRV_DIMENSION_TEXTURE1D;
	};

	template <>
	struct GetViewDimension <ID3D11Texture2D> {
		static const D3D11_SRV_DIMENSION value = D3D11_SRV_DIMENSION_TEXTURE2D;
	};

	template <>
	struct GetViewDimension <ID3D11Texture3D> {
		static const D3D11_SRV_DIMENSION value = D3D11_SRV_DIMENSION_TEXTURE3D;
	};

	template <typename TextureType>
	void CTexture<TextureType>::Initialize(const DescType& desc,
		const D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc,
		const D3D11_SUBRESOURCE_DATA* subResource,
		bool createSRV) {
		Cleanup();

		m_desc = desc;
		m_createSRV = createSRV;
		if (srvDesc) {
			m_srvDesc = *srvDesc;
		} else {
			m_srvDesc.Format = m_desc.Format;
			m_srvDesc.ViewDimension = GetViewDimension<TextureType>::value;

			m_srvDesc.Texture1D.MipLevels = m_desc.MipLevels;
			m_srvDesc.Texture1D.MostDetailedMip = 0;
			m_srvDesc.Texture1DArray.MipLevels = m_desc.MipLevels;
			m_srvDesc.Texture1DArray.MostDetailedMip = 0;

			m_srvDesc.Texture2D.MipLevels = m_desc.MipLevels;
			m_srvDesc.Texture2D.MostDetailedMip = 0;
			m_srvDesc.Texture2DArray.MipLevels = m_desc.MipLevels;
			m_srvDesc.Texture2DArray.MostDetailedMip = 0;

			m_srvDesc.Texture2DMSArray.ArraySize = 1;
			m_srvDesc.Texture2DMSArray.FirstArraySlice = 0;

			m_srvDesc.Texture3D.MipLevels = m_desc.MipLevels;
			m_srvDesc.Texture3D.MostDetailedMip = 0;

			m_srvDesc.TextureCube.MipLevels = m_desc.MipLevels;
			m_srvDesc.TextureCube.MostDetailedMip = 0;
			m_srvDesc.TextureCubeArray.MipLevels = m_desc.MipLevels;
			m_srvDesc.TextureCubeArray.MostDetailedMip = 0;
		}
		if (Application::GetDevice()) {
			if (FAILED(CreateTexture(m_desc, subResource))) {
				Application::WriteLog("バッファの作成に失敗しました");
				throw - 1;
			}
			if (m_createSRV) {
				try {
					Application::GetDevice()->CreateShaderResourceView(m_pTexture,
						&m_srvDesc, &m_pShaderResourceView);
				} catch (...) {
					Application::WriteLog("シェーダリソースビューの作成に失敗しました");
				}
			}
		} else {
			Application::WriteLog("デバイスが見つかりませんでした");
			throw - 1;
		}
	}

	template <typename TextureType>
	void CTexture<TextureType>::Cleanup() noexcept {
		SAFE_RELEASE(m_pShaderResourceView);
		SAFE_RELEASE(m_pTexture);
	}

}
