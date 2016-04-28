#include "JImage.h"
#include "JApplication.h"

using namespace Javelin;

CImage::CImage() noexcept :
m_pShaderResourceView(nullptr),
m_imageLoadInfoUsed(false){
}

CImage::~CImage() noexcept {
	Cleanup();
}

CImage::CImage(const CImage& image) {
	operator=(image);
}

CImage& CImage::operator=(const CImage& image) {
	if (this == &image) {
		return *this;
	}
	Cleanup();
	m_imageLoadInfo = image.m_imageLoadInfo;
	Initialize(image.m_filename, image.m_imageLoadInfoUsed ? &m_imageLoadInfo : nullptr);
	return *this;
}

CImage::CImage(CImage&& image) {
	operator=(std::move(image));
}

CImage& CImage::operator=(CImage&& image) {
	if (this == &image) {
		return *this;
	}
	Cleanup();
	m_pShaderResourceView = image.m_pShaderResourceView;
	m_imageInfo = image.m_imageInfo;
	m_imageLoadInfo = image.m_imageLoadInfo;
	m_imageLoadInfoUsed = image.m_imageLoadInfoUsed;
	m_filename = image.m_filename;
	return *this;
}

void CImage::Initialize(const std::string& filename, D3DX11_IMAGE_LOAD_INFO* loadInfo) {
	Cleanup();

	m_filename = filename;
	if (Application::GetDevice()) {
		if (FAILED(D3DX11CreateShaderResourceViewFromFile(
			Application::GetDevice(), filename.c_str(),
			loadInfo, nullptr, &m_pShaderResourceView, nullptr))) {
			Application::WriteLog("CImage::Initialize 画像ファイルの読み込みに失敗しました：" + filename);
			throw - 1;
		}
		if (loadInfo) {
			m_imageLoadInfo = *loadInfo;
			m_imageLoadInfoUsed = true;
		}

		if (FAILED(D3DX11GetImageInfoFromFile(filename.c_str(), nullptr, &m_imageInfo, nullptr))) {
			Application::WriteLog("CImage::Initialize 画像ファイルの情報の取得に失敗しました：" + filename);
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CImage::Cleanup() noexcept {
	SAFE_RELEASE(m_pShaderResourceView);
}
