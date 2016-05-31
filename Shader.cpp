#include "JShader.h"
#include "JApplication.h"
#include "JUtility.h"
#include <fstream>

using namespace Javelin;

CShader::CShader() noexcept :
m_pBlob(nullptr){
}

CShader::~CShader() noexcept {
	Cleanup();
}

void CShader::Initialize(const std::string& filename, const std::string& functionName,
	const std::string& shaderModel, UINT flag,
	const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude) {
	Cleanup();

	ID3DBlob* pErrorBlob;
	if (FAILED(D3DX11CompileFromFile(filename.c_str(), pMacroDefines, pInclude,
		functionName.c_str(), shaderModel.c_str(), flag, 0, nullptr, &m_pBlob, &pErrorBlob, nullptr))) {
		Application::WriteLog(std::string("シェーダのコンパイルに失敗しました：") +
			"\n\t" + filename + "\n\t" + functionName + "\n\t" + shaderModel);
		if (pErrorBlob) {
			Application::WriteLog((char*)pErrorBlob->GetBufferPointer());
			SAFE_RELEASE(pErrorBlob);
		}
		throw - 1;
	}
	SAFE_RELEASE(pErrorBlob);
	m_isPrecompiled = false;
}

void CShader::Initialize(const std::string& filename, Precompiled) {
	Cleanup();

	std::ifstream ifs(filename, std::ios::binary);
	char ch;
	while (ifs.get(ch)) {
		m_precompiledData.push_back(ch);
	}

	m_isPrecompiled = true;
}

void CShader::Cleanup() noexcept {
	m_precompiledData.clear();
	SAFE_RELEASE(m_pBlob);
}

/*
CShader::CShader(CShader&& Shader) {
	operator=(std::move(Shader));
}

CShader& CShader::operator=(CShader&& Shader) {
	if (this == &Shader) {
		return *this;
	}
	Cleanup();
	m_pBlob = Shader.m_pBlob;
	return *this;
}*/

//
//VertexShader
//

CVertexShader::CVertexShader() noexcept:
m_pShader(nullptr) {
}

CVertexShader::~CVertexShader() noexcept {
	Cleanup();
}

void CVertexShader::Initialize(const std::string& filename, const std::string& functionName,
	const std::string& shaderModel, UINT flag,
	const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, functionName, shaderModel, flag, pMacroDefines, pInclude);
		if (FAILED(Application::GetDevice()->CreateVertexShader(
			GetBufferPointer(),GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename + "\n\t" + functionName + "\n\t" + shaderModel);
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CVertexShader::Initialize(const std::string& filename, Precompiled) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, Precompiled{});
		if (FAILED(Application::GetDevice()->CreateVertexShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename);
			throw - 1;
		}
	}
	else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CVertexShader::Cleanup() noexcept {
	SAFE_RELEASE(m_pShader);
}

/*
CVertexShader::CVertexShader(CVertexShader&& Shader) {
	operator=(std::move(Shader));
}

CVertexShader& CVertexShader::operator=(CVertexShader&& Shader) {
	if (this == &Shader) {
		return *this;
	}
	Cleanup();
	m_pShader = Shader.m_pShader;
	CShader::operator=(std::move(Shader));
	return *this;
}*/

//
//GeometryShader
//

CGeometryShader::CGeometryShader() noexcept:
m_pShader(nullptr) {
}

CGeometryShader::~CGeometryShader() noexcept {
	Cleanup();
}

void CGeometryShader::Initialize(const std::string& filename, const std::string& functionName,
	const std::string& shaderModel, UINT flag,
	const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, functionName, shaderModel, flag, pMacroDefines, pInclude);
		if (FAILED(Application::GetDevice()->CreateGeometryShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename + "\n\t" + functionName + "\n\t" + shaderModel);
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CGeometryShader::Initialize(const std::string& filename, const std::string& functionName,
	const std::string& shaderModel, 
	D3D11_SO_DECLARATION_ENTRY decl[], UINT numDecl,
	UINT strides[], UINT numStrides, UINT flag,
	const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, functionName, shaderModel, flag, pMacroDefines, pInclude);
		if (FAILED(Application::GetDevice()->CreateGeometryShaderWithStreamOutput(
			GetBufferPointer(), GetBufferSize(),
			decl, numDecl, strides, numStrides,
			Application::GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? 
				D3D11_SO_NO_RASTERIZED_STREAM : 0, nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename + "\n\t" + functionName + "\n\t" + shaderModel);
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CGeometryShader::Initialize(const std::string& filename, Precompiled) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, Precompiled{});
		if (FAILED(Application::GetDevice()->CreateGeometryShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename);
			throw - 1;
		}
	}
	else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CGeometryShader::Initialize(const std::string& filename, Precompiled,
	D3D11_SO_DECLARATION_ENTRY decl[], UINT numDecl,
	UINT strides[], UINT numStrides) {
	if (Application::GetDevice()) {
		CShader::Initialize(filename, Precompiled{});
		if (FAILED(Application::GetDevice()->CreateGeometryShaderWithStreamOutput(
			GetBufferPointer(), GetBufferSize(),
			decl, numDecl, strides, numStrides,
			Application::GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ?
			D3D11_SO_NO_RASTERIZED_STREAM : 0, nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename);
			throw - 1;
		}
	}
	else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CGeometryShader::Cleanup() noexcept {
	SAFE_RELEASE(m_pShader);
}

/*
CGeometryShader::CGeometryShader(CGeometryShader&& Shader) {
	operator=(std::move(Shader));
}

CGeometryShader& CGeometryShader::operator=(CGeometryShader&& Shader) {
	if (this == &Shader) {
		return *this;
	}
	Cleanup();
	m_pShader = Shader.m_pShader;
	CShader::operator=(std::move(Shader));
	return *this;
}*/

//
//PixelShader
//

CPixelShader::CPixelShader() noexcept:
m_pShader(nullptr) {
}

CPixelShader::~CPixelShader() noexcept {
	Cleanup();
}

void CPixelShader::Initialize(const std::string& filename, const std::string& functionName,
	const std::string& shaderModel, UINT flag,
	const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, functionName, shaderModel, flag, pMacroDefines, pInclude);
		if (FAILED(Application::GetDevice()->CreatePixelShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename + "\n\t" + functionName + "\n\t" + shaderModel);
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CPixelShader::Initialize(const std::string& filename, Precompiled) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, Precompiled{});
		if (FAILED(Application::GetDevice()->CreatePixelShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename);
			throw - 1;
		}
	}
	else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CPixelShader::Cleanup() noexcept {
	SAFE_RELEASE(m_pShader);
}

/*
CPixelShader::CPixelShader(CPixelShader&& Shader) {
	operator=(std::move(Shader));
}

CPixelShader& CPixelShader::operator=(CPixelShader&& Shader) {
	if (this == &Shader) {
		return *this;
	}
	Cleanup();
	m_pShader = Shader.m_pShader;
	CShader::operator=(std::move(Shader));
	return *this;
}*/

//
//HullShader
//

CHullShader::CHullShader() noexcept:
m_pShader(nullptr) {
}

CHullShader::~CHullShader() noexcept {
	Cleanup();
}

void CHullShader::Initialize(const std::string& filename, const std::string& functionName,
	const std::string& shaderModel, UINT flag,
	const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, functionName, shaderModel, flag, pMacroDefines, pInclude);
		if (FAILED(Application::GetDevice()->CreateHullShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename + "\n\t" + functionName + "\n\t" + shaderModel);
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CHullShader::Initialize(const std::string& filename, Precompiled) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, Precompiled{});
		if (FAILED(Application::GetDevice()->CreateHullShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename);
			throw - 1;
		}
	}
	else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CHullShader::Cleanup() noexcept {
	SAFE_RELEASE(m_pShader);
}

/*
CHullShader::CHullShader(CHullShader&& Shader) {
	operator=(std::move(Shader));
}

CHullShader& CHullShader::operator=(CHullShader&& Shader) {
	if (this == &Shader) {
		return *this;
	}
	Cleanup();
	m_pShader = Shader.m_pShader;
	CShader::operator=(std::move(Shader));
	return *this;
}*/

//
//DomainShader
//

CDomainShader::CDomainShader() noexcept:
m_pShader(nullptr) {
}

CDomainShader::~CDomainShader() noexcept {
	Cleanup();
}

void CDomainShader::Initialize(const std::string& filename, const std::string& functionName,
	const std::string& shaderModel, UINT flag,
	const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, functionName, shaderModel, flag, pMacroDefines, pInclude);
		if (FAILED(Application::GetDevice()->CreateDomainShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename + "\n\t" + functionName + "\n\t" + shaderModel);
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CDomainShader::Initialize(const std::string& filename, Precompiled) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, Precompiled{});
		if (FAILED(Application::GetDevice()->CreateDomainShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename);
			throw - 1;
		}
	}
	else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CDomainShader::Cleanup() noexcept {
	SAFE_RELEASE(m_pShader);
}

/*
CDomainShader::CDomainShader(CDomainShader&& Shader) {
	operator=(std::move(Shader));
}

CDomainShader& CDomainShader::operator=(CDomainShader&& Shader) {
	if (this == &Shader) {
		return *this;
	}
	Cleanup();
	m_pShader = Shader.m_pShader;
	CShader::operator=(std::move(Shader));
	return *this;
}*/

//
//ComputeShader
//

CComputeShader::CComputeShader() noexcept:
m_pShader(nullptr) {
}

CComputeShader::~CComputeShader() noexcept {
	Cleanup();
}

void CComputeShader::Initialize(const std::string& filename, const std::string& functionName,
	const std::string& shaderModel, UINT flag,
	const D3D10_SHADER_MACRO* pMacroDefines, LPD3D10INCLUDE pInclude) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, functionName, shaderModel, flag, pMacroDefines, pInclude);
		if (FAILED(Application::GetDevice()->CreateComputeShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename + "\n\t" + functionName + "\n\t" + shaderModel);
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CComputeShader::Initialize(const std::string& filename, Precompiled) {
	Cleanup();

	if (Application::GetDevice()) {
		CShader::Initialize(filename, Precompiled{});
		if (FAILED(Application::GetDevice()->CreateComputeShader(
			GetBufferPointer(), GetBufferSize(), nullptr, &m_pShader))) {
			Application::WriteLog(std::string("シェーダオブジェクトの作成に失敗しました：") +
				"\n\t" + filename);
			throw - 1;
		}
	}
	else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CComputeShader::Cleanup() noexcept {
	SAFE_RELEASE(m_pShader);
}

/*
CComputeShader::CComputeShader(CComputeShader&& Shader) {
	operator=(std::move(Shader));
}

CComputeShader& CComputeShader::operator=(CComputeShader&& Shader) {
	if (this == &Shader) {
		return *this;
	}
	Cleanup();
	m_pShader = Shader.m_pShader;
	CShader::operator=(std::move(Shader));
	return *this;
}*/
