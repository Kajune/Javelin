#pragma once

#include "JGlobal.h"
#include "JShader.h"
#include "JPipeline.h"
#include "JTexture.h"
#include "JConstantBuffer.h"
#include "JSamplerState.h"
#include <map>
#include <vector>
#include <memory>

namespace Javelin {

	class CFont {
		HFONT		m_font;
		const UINT	m_quality;
		const UINT	m_type;

	public:
		typedef struct {
			GLYPHMETRICS gm;
			CTexture2D	tex;
		}char_t;
	private:
		mutable std::map<WCHAR, char_t>	m_fontTextureData;

		HRESULT CreateFontTexture(wchar_t ch) const;
	public:
		CFont(const std::string& fontName, int fontSize, 
			int fontWeight = 400, bool isItalic = false, UINT quality = 1, UINT type = GGO_GRAY8_BITMAP);
		const char_t& GetData(wchar_t ch) const;
		UINT GetQuality() const noexcept {
			return m_quality;
		}
	};

	class Text final : private UnConstructable {
		friend class Application;

		static CVertexShader				m_vs;
		static CPixelShader					m_ps;
		static CSamplerState				m_sampler;

		static CConstantBuffer<XMFLOAT4X4>	m_pos;
		static CConstantBuffer<COLOR>		m_color;

		static void Initialize();
	public:
		static void DrawString(float x, float y, float z, const std::string& str,
			const CFont& font, const CPipeline& pipeline, 
			const COLOR& color = COLOR(1, 1, 1, 1), float alignX = 0.0f, float alignY = 0.0f);
		static void GetWidthAndHeight(const std::string& str, const CFont& font,
			std::vector<float>& str_width, std::vector<float>& str_height,
			float& whole_width, float& whole_height);
	};

}
