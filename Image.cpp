#include "JImage.h"
#include "JApplication.h"

using namespace Javelin;

CImage::CImage() noexcept :
m_pShaderResourceView(nullptr),
m_imageLoadInfoUsed(false),
m_loadFromTexture(false),
m_texture(nullptr){
}

CImage::~CImage() noexcept {
	Cleanup();
}

CImage::CImage(const CImage& image) :
m_pShaderResourceView(nullptr),
m_imageLoadInfoUsed(false),
m_loadFromTexture(false),
m_texture(nullptr) {
	operator=(image);
}

CImage& CImage::operator=(const CImage& image) {
	if (this == &image) {
		return *this;
	}
	Cleanup();
	if (!image.m_pShaderResourceView) {
		return *this;
	}
	if (image.m_loadFromTexture) {
		Initialize(image.m_texture);
	} else {
		m_imageLoadInfo = image.m_imageLoadInfo;
		Initialize(image.m_filename, image.m_imageLoadInfoUsed ? &m_imageLoadInfo : nullptr);
	}
	return *this;
}

CImage::CImage(CImage&& image) :
m_pShaderResourceView(nullptr),
m_imageLoadInfoUsed(false),
m_loadFromTexture(false),
m_texture(nullptr) {
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
	m_loadFromTexture = image.m_loadFromTexture;
	m_texture = image.m_texture;
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

void CImage::Initialize(ID3D11Texture2D* texture) {
	Cleanup();

	if (Application::GetDevice()) {
		m_texture = texture;
		m_loadFromTexture = true;
		if (FAILED(Application::GetDevice()->CreateShaderResourceView(texture, nullptr, &m_pShaderResourceView))) {
			Application::WriteLog("CImage::Initialize シェーダリソースビューの作成に失敗しました");
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

/*
void CImage::LoadDivImage(std::string filename,
	float sizeX, float sizeY,
	UINT divX, UINT divY,
	UINT begin, UINT end,
	CImage imageList[],
	D3DX11_IMAGE_LOAD_INFO* loadInfo) {
	if (end < begin) {
		Application::WriteLog("LoadDivImage beginとendの値が不正です：" + filename);
		return;
	}
	if (divX * divY <= 0) {
		Application::WriteLog("LoadDivImage divXまたはdivYの値が不正です：" + filename);
		return;
	}
	if (sizeX <= 0.0f || sizeY <= 0.0f) {
		Application::WriteLog("LoadDivImage sizeXまたはsizeYの値が不正です：" + filename);
		return;
	}
	if (divX * divY < end - begin) {
		Application::WriteLog("LoadDivImage divX, divYまたはbegin, endの値が不正です：" + filename);
		return;
	}

	D3DX11_IMAGE_INFO imageInfo;
	if (FAILED(D3DX11GetImageInfoFromFile(filename.c_str(), nullptr, &imageInfo, nullptr))) {
		Application::WriteLog("CImage::Initialize 画像ファイルの情報の取得に失敗しました：" + filename);
		throw - 1;
	}

	if (divX * sizeX > imageInfo.Width || divY * sizeY > imageInfo.Height) {
		Application::WriteLog("LoadDivImage 指定された分割数と分割サイズが画像のサイズを上回っています：" + filename);
		return;
	}

	ID3D11Resource* pImage;
	if (FAILED(D3DX11CreateTextureFromFile(Application::GetDevice(), filename.c_str(),
		loadInfo, nullptr, &pImage, nullptr))) {
		Application::WriteLog("LoadDivImage ファイルの読み込みに失敗しました：" + filename);
		throw - 1;
	}

	ID3D11Texture2D* pDivImage = nullptr;
	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Format = imageInfo.Format;
	desc.Width = (UINT)sizeX;
	desc.Height = (UINT)sizeY;
	desc.MipLevels = imageInfo.MipLevels;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	Application::GetDevice()->CreateTexture2D(&desc, nullptr, &pDivImage);

	D3DX11_TEXTURE_LOAD_INFO divInfo;
	D3D11_BOX box;
	box.front = 0;
	box.back = 1;
	divInfo.pSrcBox = &box;
	for (UINT i = begin; i < end; i++) {
		float offsetX = sizeX * float(i % divX);
		float offsetY = sizeY * float(i / divY);
		box.left = UINT(offsetX);
		box.right = UINT(offsetX + sizeX);
		box.top = UINT(offsetY);
		box.bottom = UINT(offsetY + sizeY);
		D3DX11LoadTextureFromTexture(Application::GetDeferredContext().get(), pImage, &divInfo, pDivImage);
		CImage image;
		image.Initialize(pDivImage);
		imageList[i - begin] = image;
	}
	SAFE_RELEASE(pDivImage);
	SAFE_RELEASE(pImage);
}*/
