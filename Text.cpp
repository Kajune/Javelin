#include "JText.h"
#include "JApplication.h"
#include "JUtility.h"
#include "JConstantBuffer.hpp"
#include "JPipeline.hpp"
#include <cstdarg>

using namespace Javelin;

CVertexShader						Text::m_vs;
CPixelShader						Text::m_ps;
CSamplerState						Text::m_sampler;

CConstantBuffer<XMFLOAT4X4>			Text::m_pos;
CConstantBuffer<COLOR>				Text::m_color;

namespace {
	constexpr int num_bitmaps[] = {
		1, 1, 1, 1, 4, 16, 64,
	};
}

CFont::CFont(const std::string& fontName, int fontSize, int fontWeight, bool isItalic, UINT quality, UINT type):
m_quality(quality), m_type(type){
	LOGFONT lf;
	lf.lfHeight = fontSize * quality;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = fontWeight;
	lf.lfItalic = (BYTE)isItalic;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = SHIFTJIS_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	std::copy(fontName.begin(), fontName.end(), lf.lfFaceName);
	m_font = CreateFontIndirect(&lf);
}

HRESULT CFont::CreateFontTexture(wchar_t ch) const {
	HDC hdc = GetDC(nullptr);
	HFONT oldFont = (HFONT)SelectObject(hdc, m_font);

	GLYPHMETRICS GM;
	const MAT2 mat = { {0, 1}, {0, 0}, {0, 0}, {0, 1} };

	DWORD size = GetGlyphOutlineW(hdc, ch, m_type, &GM, 0, nullptr, &mat);
	if (size <= 0) {
		size = 1;
	}
	std::vector<BYTE> pFontData(size);
	std::fill(pFontData.begin(), pFontData.end(), 0);
	GetGlyphOutlineW(hdc, ch, m_type, &GM, size, pFontData.data(), &mat);

	SelectObject(hdc, oldFont);
	ReleaseDC(nullptr, hdc);

	m_fontTextureData.emplace(std::piecewise_construct, std::make_tuple(ch), std::make_tuple());
	m_fontTextureData.at(ch).gm = GM;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = GM.gmBlackBoxX;
	desc.Height = GM.gmBlackBoxY;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8_UNORM;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	std::vector<BYTE> pFontDataf;
	for (auto& it : pFontData) {
		pFontDataf.emplace_back(it * 255 / num_bitmaps[m_type]);
	}

	D3D11_SUBRESOURCE_DATA sub;
	sub.pSysMem = pFontDataf.data();
	sub.SysMemPitch = sizeof(BYTE) * ceil_to_multiple(desc.Width, 4);
	sub.SysMemSlicePitch = 0;

	m_fontTextureData.at(ch).tex.Initialize(desc, &srvDesc, &sub, true);

	return S_OK;
}

const CFont::char_t& CFont::GetData(wchar_t ch) const {
	if (m_fontTextureData.find(ch) == m_fontTextureData.end()) {
		CreateFontTexture(ch);
	}
	return m_fontTextureData.at(ch);
}

void Text::Initialize() {
#if defined(_WIN64)
	m_vs.Initialize("shader/JTextShader_VS_64.cfx", Precompiled{});
	m_ps.Initialize("shader/JTextShader_PS_64.cfx", Precompiled{});
#else
	m_vs.Initialize("shader/JTextShader_VS_86.cfx", Precompiled{});
	m_ps.Initialize("shader/JTextShader_PS_86.cfx", Precompiled{});
#endif

	m_sampler.Initialize(D3D11_TEXTURE_ADDRESS_CLAMP);

	m_pos.Initialize();
	m_color.Initialize();
}

void Text::GetWidthAndHeight(const std::string& str, const CFont& font,
	std::vector<float>& str_width, std::vector<float>& str_height,
	float& whole_width, float& whole_height) {

	std::wstring wstr(str_to_wstr(str));

	str_width.resize(1);
	str_height.resize(1);
	UINT line = 0;

	UINT width, height;
	Application::GetScreenSize(width, height);
	width *= font.GetQuality();
	height *= font.GetQuality();

	whole_width = whole_height = 0.0f;

	for (const auto& it : wstr) {
		if (it == '\n') {
			whole_width = max(whole_width, str_width.at(line));
			whole_height += str_height.at(line);
			str_height.emplace_back(0.0f);
			str_width.emplace_back(0.0f);
			line++;
			continue;
		}
		try {
			auto& ch = font.GetData(it);
			str_height.at(line) = max(str_height.at(line), ch.gm.gmptGlyphOrigin.y * 2.0f / (float)height);
			str_width.at(line) += ch.gm.gmCellIncX * 2.0f / (float)width;
		}
		catch (...) {
			continue;
		}
	}
	whole_width = max(whole_width, str_width.at(line));
	whole_height += str_height.at(line);
}

void Text::DrawString(float x, float y, float z, const std::string& str,
	const CFont& font, const CPipeline& pipeline, const COLOR& color, float alignX, float alignY) {
	std::wstring wstr(str_to_wstr(str));

	pipeline.BeginStorePipeline();
	pipeline.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pipeline.SetInputLayout(nullptr);
	pipeline.SetVertexShader(&m_vs);
	pipeline.SetGeometryShader(nullptr);
	pipeline.SetPixelShader(&m_ps);
	pipeline.SetPixelShaderSamplerState(0, &m_sampler);
	pipeline.SetVertexShaderConstantBuffer(0, &m_pos);
	m_color.UpdateBufferValue(color, Application::GetImmediateContext());
	pipeline.SetPixelShaderConstantBuffer(1, &m_color);

	std::vector<float> str_height;
	std::vector<float> str_width;
	float whole_width, whole_height;
	GetWidthAndHeight(str, font, str_width, str_height, whole_width, whole_height);

	UINT line = 0;
	UINT width, height;
	Application::GetScreenSize(width, height);
	float lastPosX = 0.0f;
	float lastPosY = str_height.at(0);

	for (const auto& it : wstr) {
		if (it == '\n') {
			lastPosX = 0.0f;
			line++;
			lastPosY += str_height.at(line);
			continue;
		}

		float sizeX;
		float sizeY;

		try {
			const CFont::char_t& ch = font.GetData(it);

			pipeline.SetPixelShaderResource(0, &ch.tex);

			static int index = 0;

			sizeX = (float)ch.gm.gmBlackBoxX / (float)width;
			sizeY = (float)ch.gm.gmBlackBoxY / (float)height;
			XMFLOAT4X4 mat;
			XMStoreFloat4x4(&mat, XMMatrixTranspose(
				XMMatrixScaling(sizeX, sizeY, 1.0f)
				* XMMatrixTranslation(
					x + lastPosX + sizeX + (float)ch.gm.gmptGlyphOrigin.x / (float)width
					- alignX * str_width.at(line),
					-(y + lastPosY + sizeY - (float)ch.gm.gmptGlyphOrigin.y * 2.0f / (float)height
					- alignY * whole_height),
					z)
			));
			m_pos.UpdateBufferValue(mat, Application::GetImmediateContext());
			lastPosX += (float)ch.gm.gmCellIncX * 2.0f / (float)width;
		} catch (...) {
			continue;
		}

		pipeline.Draw(6);

	}

	pipeline.RestorePipeline();
}
