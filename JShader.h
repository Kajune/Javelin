#pragma once

#include "JResource.h"
#include <string>

namespace Javelin {

	enum class ShaderCompileOption : UINT {
		DEBUG = D3D10_SHADER_DEBUG |
				D3D10_SHADER_SKIP_OPTIMIZATION |
				D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR |
				D3D10_SHADER_ENABLE_STRICTNESS,
		DEVELOP = D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR,
		RELEASE = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR,
	};

	class CShader : public CResource {
	protected:
		ID3DBlob*			m_pBlob;
	public:
		CShader() noexcept;
		virtual ~CShader() noexcept;
		CShader(const CShader&) = delete;
		CShader& operator=(const CShader&) = delete;
		CShader(CShader&&);
		CShader& operator=(CShader&&);

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag,
			const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude);
		void Cleanup() noexcept;

		ID3DBlob* GetBlob() const noexcept {
			return m_pBlob;
		}
	};

	class CVertexShader : public CShader {
	protected:
		ID3D11VertexShader*	m_pShader;
	public:
		CVertexShader() noexcept;
		virtual ~CVertexShader() noexcept;
		CVertexShader(CVertexShader&&);
		CVertexShader& operator=(CVertexShader&&);

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag, 
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Cleanup() noexcept;

		ID3D11VertexShader* GetShader() const noexcept {
			return m_pShader;
		}
	};

	class CGeometryShader : public CShader {
	protected:
		ID3D11GeometryShader*	m_pShader;
	public:
		CGeometryShader() noexcept;
		virtual ~CGeometryShader() noexcept;
		CGeometryShader(CGeometryShader&&);
		CGeometryShader& operator=(CGeometryShader&&);

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Cleanup() noexcept;

		ID3D11GeometryShader* GetShader() const noexcept {
			return m_pShader;
		}
	};

	class CPixelShader : public CShader {
	protected:
		ID3D11PixelShader*	m_pShader;
	public:
		CPixelShader() noexcept;
		virtual ~CPixelShader() noexcept;
		CPixelShader(CPixelShader&&);
		CPixelShader& operator=(CPixelShader&&);

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Cleanup() noexcept;

		ID3D11PixelShader* GetShader() const noexcept {
			return m_pShader;
		}
	};

	class CHullShader : public CShader {
	protected:
		ID3D11HullShader*	m_pShader;
	public:
		CHullShader() noexcept;
		virtual ~CHullShader() noexcept;
		CHullShader(CHullShader&&);
		CHullShader& operator=(CHullShader&&);

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Cleanup() noexcept;

		ID3D11HullShader* GetShader() const noexcept {
			return m_pShader;
		}
	};

	class CDomainShader : public CShader {
	protected:
		ID3D11DomainShader*	m_pShader;
	public:
		CDomainShader() noexcept;
		virtual ~CDomainShader() noexcept;
		CDomainShader(CDomainShader&&);
		CDomainShader& operator=(CDomainShader&&);

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Cleanup() noexcept;

		ID3D11DomainShader* GetShader() const noexcept {
			return m_pShader;
		}
	};

	class CComputeShader : public CShader {
	protected:
		ID3D11ComputeShader*	m_pShader;
	public:
		CComputeShader() noexcept;
		virtual ~CComputeShader() noexcept;
		CComputeShader(CComputeShader&&);
		CComputeShader& operator=(CComputeShader&&);

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Cleanup() noexcept;

		ID3D11ComputeShader* GetShader() const noexcept {
			return m_pShader;
		}
	};

}

