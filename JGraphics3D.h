#pragma once
#include "JGlobal.h"
#include "JShader.h"
#include "JInputLayout.h"
#include "JDepthStencil.h"
#include "JViewport.h"
#include "JSamplerState.h"
#include "JImage.h"
#include "JConstantBuffer.h"
#include "JCamera.h"
#include "JMesh.h"
#include "JPipeline.h"

namespace Javelin {

	class Graphics3D {
		friend class Application;
	public:
		static const UINT kMaxLightNum = 16;
	private:

		static CVertexShader m_vs, m_vs_shadow;
		static CPixelShader m_ps;

		static CInputLayout m_layout;

		static CDepthStencil m_shadowBuffer;
		static CViewport m_viewport;

		static CSamplerState m_sampler;
		static CImage m_defaultTexture;

		static struct cbObj_t {
			XMFLOAT4X4 World;
			XMFLOAT4X4 ViewProjection;
			XMFLOAT4X4 ViewProjectionShadow;
		}m_cbObjInst;

		static struct cbMat_t {
			COLOR diffuse, specular, ambient, opacity_reflection_refraction;
		}m_cbMatInst;

		static struct cbLight_t {
			XMFLOAT3 eyePos;
			UINT numLights;
			XMFLOAT3 ambientLight;
			float dummy;
			XMFLOAT4 lightPower[kMaxLightNum];
			XMFLOAT4 lightPos[kMaxLightNum];
			COLOR specularLight[kMaxLightNum];
			COLOR diffuseLight[kMaxLightNum];
		}m_cbLightInst;

		static CConstantBuffer<cbObj_t> m_cbObj;
		static CConstantBuffer<cbMat_t> m_cbMat;
		static CConstantBuffer<cbLight_t> m_cbLight;

		static CCamera m_shadowCamera;

		static bool m_drawShadow;

		static void Initialize();
	public:
		static void DrawMesh(const CMesh<>& mesh, const CCamera& camera, 
			const XMFLOAT4X4& mat, const CPipeline& pipeline);
		static void DrawShadowBuffer(const CMesh<>& mesh, const CCamera& camera, 
			const XMFLOAT4X4& mat, const CPipeline& pipeline);
		static void ClearShadowBuffer();

		static void SetLightParam(
			UINT numLights,
			const XMFLOAT3 LightPos[], 
			const COLOR SpecularColor[], 
			const COLOR DiffuseColor[],
			const float LightPower[],
			const COLOR& AmbientColor);
		static void SetShadowCameraParam(float fov, float nearZ, float farZ);
		static void SetShadowCameraTransform(const XMFLOAT3& LightPos, const XMFLOAT3& TargetPos);
		static void SetShadowBufferSize(UINT size);
		static void SetDrawShadow(bool drawShadow) noexcept {
			m_drawShadow = drawShadow;
		}
	};

}
