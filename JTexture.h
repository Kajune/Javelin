#pragma once

#include "JShaderResourceView.h"

namespace Javelin {
	template<typename TextureType>
	struct TextureDescType {
		using Type = void;
	};

	template <typename TextureType>
	class CTexture : public CShaderResourceView {
	protected:
		using DescType = typename TextureDescType<TextureType>::Type;
		TextureType*					m_pTexture;
		ID3D11ShaderResourceView*		m_pShaderResourceView;
		DescType						m_desc;
		D3D11_SHADER_RESOURCE_VIEW_DESC	m_srvDesc;
		bool							m_createSRV;

		HRESULT CreateTexture(const DescType& desc,
			const D3D11_SUBRESOURCE_DATA* subResource);
	public:
		CTexture() noexcept;
		virtual ~CTexture() noexcept;

		void Initialize(const DescType& desc,
			const D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc,
			const D3D11_SUBRESOURCE_DATA* subResource,
			bool createSRV);
		void Cleanup() noexcept;

		TextureType* GetTexture() const noexcept {
			return m_pTexture;
		}
		ID3D11ShaderResourceView* GetShaderResourceView() const noexcept override {
			return m_pShaderResourceView;
		}
		operator bool() const override {
			return m_pTexture && m_pShaderResourceView;
		}
	};

	using CTexture1D = CTexture<ID3D11Texture1D>;
	using CTexture2D = CTexture<ID3D11Texture2D>;
	using CTexture3D = CTexture<ID3D11Texture3D>;

	template <>
	struct TextureDescType<ID3D11Texture1D> {
		using Type = D3D11_TEXTURE1D_DESC;
	};

	template <>
	struct TextureDescType<ID3D11Texture2D> {
		using Type = D3D11_TEXTURE2D_DESC;
	};

	template <>
	struct TextureDescType<ID3D11Texture3D> {
		using Type = D3D11_TEXTURE3D_DESC;
	};
}

