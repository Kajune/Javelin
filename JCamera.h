#pragma once

#include "JGlobal.h"

namespace Javelin {

	class CCamera {
		mutable XMFLOAT4X4 View, Projection, ViewProjection;
		mutable XMMATRIX ViewMat, ProjMat;

		float m_fov, m_aspect, m_nearZ, m_farZ;

		XMVECTOR m_CamPos, m_TargetPos, m_UpVec;

		mutable bool m_changedView, m_changedProjection;

		void Calc() const noexcept {
			if (m_changedView) {
				ViewMat = XMMatrixLookAtLH(m_CamPos, m_TargetPos, m_UpVec);
				XMStoreFloat4x4(&View, XMMatrixTranspose(ViewMat));
			}
			if (m_changedProjection) {
				ProjMat = XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_nearZ, m_farZ);
				XMStoreFloat4x4(&Projection, XMMatrixTranspose(ProjMat));
			}
			if (m_changedView || m_changedProjection) {
				XMStoreFloat4x4(&ViewProjection, XMMatrixTranspose(ViewMat * ProjMat));
			}
			m_changedView = false;
			m_changedProjection = false;
		}
	public:
		CCamera(float fov, float aspect, float nearZ, float farZ) :
			m_fov(fov), m_aspect(aspect), m_nearZ(nearZ), m_farZ(farZ),
			m_changedView(true), m_changedProjection(true) {
			m_CamPos = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
			m_TargetPos = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
			m_UpVec = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
		}

		void SetFov(float fov) noexcept {
			m_fov = fov;
			m_changedProjection = true;
		}
		void SetAspect(float aspect) noexcept {
			m_aspect = aspect;
			m_changedProjection = true;
		}
		void SetNearZ(float nearZ) noexcept {
			m_nearZ = nearZ;
			m_changedProjection = true;
		}
		void SetFarZ(float farZ) noexcept {
			m_farZ = farZ;
			m_changedProjection = true;
		}

		void SetCamPos(const XMFLOAT3& camPos) noexcept{
			m_CamPos = XMLoadFloat3(&camPos);
			m_changedView = true;
		}
		void SetTargetPos(const XMFLOAT3& targetPos) noexcept {
			m_TargetPos = XMLoadFloat3(&targetPos);
			m_changedView = true;
		}
		void SetUpVec(const XMFLOAT3& upVec) noexcept {
			m_UpVec = XMLoadFloat3(&upVec);
			m_changedView = true;
		}

		XMFLOAT3 GetCamPos() const noexcept {
			XMFLOAT3 ret;
			XMStoreFloat3(&ret, m_CamPos);
			return ret;
		}

		XMFLOAT3 GetTargetPos() const noexcept {
			XMFLOAT3 ret;
			XMStoreFloat3(&ret, m_TargetPos);
			return ret;
		}

		XMFLOAT3 GetUpVec() const noexcept {
			XMFLOAT3 ret;
			XMStoreFloat3(&ret, m_UpVec);
			return ret;
		}

		XMFLOAT4X4 GetProjection() const noexcept {
			Calc();
			return Projection;
		}
		XMFLOAT4X4 GetView() const noexcept {
			Calc();
			return View;
		}
		XMFLOAT4X4 GetViewProjection() const noexcept {
			Calc();
			return ViewProjection;
		}
	};

}
