#pragma once

#include "JResource.h"

namespace Javelin {
	template<typename TextureType>
	struct TextureDescType {
		using Type = void;
	};

	template <typename TextureType>
	class CTexture : public CResource {
	protected:
		using DescType = typename TextureDescType<TextureType>::Type;
		TextureType*					m_pTexture;
		ID3D11ShaderResourceView*		m_pShaderResourceView;
		DescType						m_desc;
		D3D11_SHADER_RESOURCE_VIEW_DESC	m_srvDesc;

		HRESULT CreateTexture(const DescType& desc,
			const D3D11_SUBRESOURCE_DATA* subResource);
	public:
		CTexture() noexcept;
		virtual ~CTexture() noexcept;
		CTexture(const CTexture<TextureType>& texture);
		CTexture& operator=(const CTexture<TextureType>& texture);
		CTexture(CTexture<TextureType>&& texture);
		CTexture& operator=(CTexture<TextureType>&& texture);

		void Initialize(const DescType& desc,
			const D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc,
			const D3D11_SUBRESOURCE_DATA* subResource);
		void Cleanup() noexcept;

		TextureType* GetTexture() const noexcept {
			return m_pTexture;
		}
		ID3D11ShaderResourceView* GetShaderResourceView() const noexcept {
			return m_pShaderResourceView;
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

