#pragma once

#include "JResource.h"
#include <string>
#include <vector>

namespace Javelin {

	enum ShaderCompileOption : UINT {
		DEBUG = D3D10_SHADER_DEBUG |
				D3D10_SHADER_SKIP_OPTIMIZATION |
				D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR |
				D3D10_SHADER_ENABLE_STRICTNESS,
		DEVELOP = D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR,
		RELEASE = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR,
#if defined(DEBUG) || defined(_DEBUG)
		RECOMMENDED = ShaderCompileOption::DEBUG,
#else
		RECOMMENDED = ShaderCompileOption::RELEASE,
#endif
	};

	struct Precompiled {};

	class CShader : public CResource {
		ID3DBlob*			m_pBlob;
		bool				m_isPrecompiled;
		std::vector<BYTE>	m_precompiledData;
	public:
		CShader() noexcept;
		virtual ~CShader() noexcept;

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag,
			const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude);
		void Initialize(const std::string& filename, Precompiled);
		void Cleanup() noexcept;

		const void* GetBufferPointer() const noexcept {
			return (m_isPrecompiled ? m_precompiledData.data() : m_pBlob->GetBufferPointer());
		}
		size_t GetBufferSize() const noexcept {
			return (m_isPrecompiled ? sizeof(BYTE) * m_precompiledData.size() : m_pBlob->GetBufferSize());
		}
		operator bool() const override {
			return m_pBlob != nullptr;
		}
	};

	class CVertexShader : public CShader {
	protected:
		ID3D11VertexShader*	m_pShader;
	public:
		CVertexShader() noexcept;
		virtual ~CVertexShader() noexcept;

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag = ShaderCompileOption::RECOMMENDED, 
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Initialize(const std::string& filename, Precompiled);
		void Cleanup() noexcept;

		ID3D11VertexShader* GetShader() const noexcept {
			return m_pShader;
		}
		operator bool() const override {
			return m_pShader != nullptr && CShader::operator bool();
		}
	};

	class CGeometryShader : public CShader {
	protected:
		ID3D11GeometryShader*	m_pShader;
	public:
		CGeometryShader() noexcept;
		virtual ~CGeometryShader() noexcept;

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag = ShaderCompileOption::RECOMMENDED,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, 
			D3D11_SO_DECLARATION_ENTRY decl[], UINT numDecl,
			UINT strides[], UINT numStrides,
			UINT flag = ShaderCompileOption::RECOMMENDED,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Initialize(const std::string& filename, Precompiled);
		void Initialize(const std::string& filename, Precompiled,
			D3D11_SO_DECLARATION_ENTRY decl[], UINT numDecl,
			UINT strides[], UINT numStrides);
		void Cleanup() noexcept;

		ID3D11GeometryShader* GetShader() const noexcept {
			return m_pShader;
		}
		operator bool() const override {
			return m_pShader != nullptr && CShader::operator bool();
		}
	};

	class CPixelShader : public CShader {
	protected:
		ID3D11PixelShader*	m_pShader;
	public:
		CPixelShader() noexcept;
		virtual ~CPixelShader() noexcept;

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag = ShaderCompileOption::RECOMMENDED,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Initialize(const std::string& filename, Precompiled);
		void Cleanup() noexcept;

		ID3D11PixelShader* GetShader() const noexcept {
			return m_pShader;
		}
		operator bool() const override {
			return m_pShader != nullptr && CShader::operator bool();
		}
	};

	class CHullShader : public CShader {
	protected:
		ID3D11HullShader*	m_pShader;
	public:
		CHullShader() noexcept;
		virtual ~CHullShader() noexcept;

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag = ShaderCompileOption::RECOMMENDED,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Initialize(const std::string& filename, Precompiled);
		void Cleanup() noexcept;

		ID3D11HullShader* GetShader() const noexcept {
			return m_pShader;
		}
		operator bool() const override {
			return m_pShader != nullptr && CShader::operator bool();
		}
	};

	class CDomainShader : public CShader {
	protected:
		ID3D11DomainShader*	m_pShader;
	public:
		CDomainShader() noexcept;
		virtual ~CDomainShader() noexcept;

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag = ShaderCompileOption::RECOMMENDED,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Initialize(const std::string& filename, Precompiled);
		void Cleanup() noexcept;

		ID3D11DomainShader* GetShader() const noexcept {
			return m_pShader;
		}
		operator bool() const override {
			return m_pShader != nullptr && CShader::operator bool();
		}
	};

	class CComputeShader : public CShader {
	protected:
		ID3D11ComputeShader*	m_pShader;
	public:
		CComputeShader() noexcept;
		virtual ~CComputeShader() noexcept;

		void Initialize(const std::string& filename, const std::string& functionName,
			const std::string& shaderModel, UINT flag = ShaderCompileOption::RECOMMENDED,
			const D3D10_SHADER_MACRO* pMacroDefines = nullptr, LPD3D10INCLUDE pInclude = nullptr);
		void Initialize(const std::string& filename, Precompiled);
		void Cleanup() noexcept;

		ID3D11ComputeShader* GetShader() const noexcept {
			return m_pShader;
		}
		operator bool() const override {
			return m_pShader != nullptr && CShader::operator bool();
		}
	};

}

