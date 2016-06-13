#include "JGraphics3D.h"
#include "JUtility.h"
#include "JApplication.h"
#include "JConstantBuffer.hpp"
#include "JPipeline.hpp"

using namespace Javelin;

CVertexShader Graphics3D::m_vs, Graphics3D::m_vs_shadow;
CPixelShader Graphics3D::m_ps;

CInputLayout Graphics3D::m_layout;

CDepthStencil Graphics3D::m_shadowBuffer;
CViewport Graphics3D::m_viewport;

CSamplerState Graphics3D::m_sampler;
CImage Graphics3D::m_defaultTexture;

struct Graphics3D::cbObj_t Graphics3D::m_cbObjInst;
struct Graphics3D::cbMat_t Graphics3D::m_cbMatInst;
struct Graphics3D::cbLight_t Graphics3D::m_cbLightInst;

CConstantBuffer<Graphics3D::cbObj_t> Graphics3D::m_cbObj;
CConstantBuffer<Graphics3D::cbMat_t> Graphics3D::m_cbMat;
CConstantBuffer<Graphics3D::cbLight_t> Graphics3D::m_cbLight;

CCamera Graphics3D::m_shadowCamera(J_PI / 3.0f, 1.0f, 0.2f, 1000.0f);

bool Graphics3D::m_drawShadow = true;

void Graphics3D::Initialize() {
	m_vs.Initialize("JDefault3DShader.hlsl", "VS", "vs_4_0");
	m_vs_shadow.Initialize("JDefault3DShader.hlsl", "VS_Shadow", "vs_4_0");
	m_ps.Initialize("JDefault3DShader.hlsl", "PS", "ps_4_0");

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(XMFLOAT3) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_layout.Initialize(inputDesc, array_length(inputDesc), m_vs);

	SetShadowBufferSize(2048);

	m_sampler.Initialize(D3D11_TEXTURE_ADDRESS_WRAP);

	m_defaultTexture.Initialize("resource/white.png");

	m_cbObj.Initialize();
	m_cbMat.Initialize();
	m_cbLight.Initialize();
}

void Graphics3D::SetShadowBufferSize(UINT size) {
	m_shadowBuffer.Initialize(size, size);
	m_viewport.Initialize(size, size);
}

void Graphics3D::SetLightParam(
	UINT numLights,
	const XMFLOAT3 LightPos[],
	const COLOR SpecularColor[],
	const COLOR DiffuseColor[],
	const float LightPower[],
	const COLOR& AmbientColor) {
	m_cbLightInst.numLights = min(numLights, kMaxLightNum);
	m_cbLightInst.ambientLight = XMFLOAT3(AmbientColor.ary.rgba);
	for (UINT i = 0; i < m_cbLightInst.numLights; i++) {
		m_cbLightInst.lightPos[i] = XMFLOAT4(LightPos[i].x, LightPos[i].y, LightPos[i].z, 0.0f);
		m_cbLightInst.specularLight[i] = SpecularColor[i];
		m_cbLightInst.diffuseLight[i] = DiffuseColor[i];
		m_cbLightInst.lightPower[i] = XMFLOAT4(LightPower[i], LightPower[i], LightPower[i], LightPower[i]);
	}

	m_cbLight.UpdateBufferValue(m_cbLightInst, Application::GetImmediateContext());
}

void Graphics3D::SetShadowCameraParam(float fov, float nearZ, float farZ) {
	m_shadowCamera.SetFov(fov);
	m_shadowCamera.SetNearZ(nearZ);
	m_shadowCamera.SetFarZ(farZ);
}

void Graphics3D::SetShadowCameraTransform(const XMFLOAT3& LightPos, const XMFLOAT3& TargetPos) {
	m_shadowCamera.SetCamPos(LightPos);
	m_shadowCamera.SetTargetPos(TargetPos);
}

void Graphics3D::DrawMesh(const CMesh<>& mesh, const CCamera& camera, 
	const XMFLOAT4X4& mat, const CPipeline& pipeline) {
	m_cbObjInst.ViewProjection = camera.GetViewProjection();
	m_cbObjInst.ViewProjectionShadow = m_shadowCamera.GetViewProjection();
	m_cbObjInst.World = mat;

	m_cbObj.UpdateBufferValue(m_cbObjInst, Application::GetImmediateContext());

	if (m_drawShadow) {
		m_cbLightInst.eyePos = camera.GetCamPos();
		m_cbLight.UpdateBufferValue(m_cbLightInst, Application::GetImmediateContext());
	}

	pipeline.BeginStorePipeline();

	pipeline.SetInputLayout(&m_layout);

	pipeline.SetVertexShader(&m_vs);
	pipeline.SetPixelShader(&m_ps);

	pipeline.SetPixelShaderResource(7, &m_shadowBuffer);

	pipeline.SetPixelShaderSamplerState(0, &m_sampler);

	pipeline.SetVertexShaderConstantBuffer(0, &m_cbObj);
	pipeline.SetPixelShaderConstantBuffer(1, &m_cbMat);
	pipeline.SetPixelShaderConstantBuffer(2, &m_cbLight);

	for (auto it = mesh.begin(); it != mesh.end(); it++) {
		pipeline.SetVertexBuffer(0, &it->vertex);
		pipeline.SetIndexBuffer(&it->index, 0);

		m_cbMatInst.diffuse = mesh.GetMaterial(it).materialParam.diffuse;
		m_cbMatInst.specular = mesh.GetMaterial(it).materialParam.specular;
		m_cbMatInst.ambient = mesh.GetMaterial(it).materialParam.ambient;
		m_cbMatInst.opacity_reflection_refraction
			= COLOR(mesh.GetMaterial(it).materialParam.opacity,
				mesh.GetMaterial(it).materialParam.reflection,
				mesh.GetMaterial(it).materialParam.refraction, 0.0f);
		m_cbMat.UpdateBufferValue(m_cbMatInst, Application::GetImmediateContext());

		pipeline.SetPixelShaderResource(0,
			(mesh.GetMaterial(it).diffuseMap ? &mesh.GetMaterial(it).diffuseMap : &m_defaultTexture));
		pipeline.SetPixelShaderResource(1,
			(mesh.GetMaterial(it).specularMap ? &mesh.GetMaterial(it).specularMap :
			(mesh.GetMaterial(it).diffuseMap ? &mesh.GetMaterial(it).diffuseMap : &m_defaultTexture)));
		pipeline.SetPixelShaderResource(2,
			(mesh.GetMaterial(it).ambientMap ? &mesh.GetMaterial(it).ambientMap :
			(mesh.GetMaterial(it).diffuseMap ? &mesh.GetMaterial(it).diffuseMap : &m_defaultTexture)));

		pipeline.DrawIndexed(it->index.GetBufferLength());
	}

	pipeline.RestorePipeline();
}

void Graphics3D::DrawShadowBuffer(const CMesh<>& mesh, const CCamera& camera, 
	const XMFLOAT4X4& mat, const CPipeline& pipeline) {
	if (!m_drawShadow) {
		return;
	}

	m_cbObjInst.ViewProjection = camera.GetViewProjection();
	m_cbObjInst.ViewProjectionShadow = m_shadowCamera.GetViewProjection();
	m_cbObjInst.World = mat;

	m_cbObj.UpdateBufferValue(m_cbObjInst, Application::GetImmediateContext());

	pipeline.BeginStorePipeline();

	pipeline.SetPixelShaderResource(7, nullptr);

	pipeline.SetInputLayout(&m_layout);

	pipeline.SetVertexShader(&m_vs_shadow);
	pipeline.SetPixelShader(nullptr);

	pipeline.SetRenderTarget(nullptr, &m_shadowBuffer);
	pipeline.SetViewports(&m_viewport);

	pipeline.SetVertexShaderConstantBuffer(0, &m_cbObj);

	for (auto it = mesh.begin(); it != mesh.end(); it++) {
		pipeline.SetVertexBuffer(0, &it->vertex);
		pipeline.SetIndexBuffer(&it->index, 0);

		pipeline.DrawIndexed(it->index.GetBufferLength());
	}

	pipeline.RestorePipeline();
}

void Graphics3D::ClearShadowBuffer() {
	Application::ClearScreen(nullptr, &m_shadowBuffer, COLOR(1.0f, 1.0f, 1.0f, 1.0f));
}
