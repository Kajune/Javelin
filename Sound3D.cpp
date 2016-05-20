#include "JSound3D.h"
#include "JVoice.h"

using namespace Javelin;

#pragma comment( lib, "x3daudio.lib" )

X3DAUDIO_HANDLE	CSound3D::m_X3DInstance = "";
std::list<CSound3D*> CSound3D::sound3DList;
X3DAUDIO_LISTENER CSound3D::m_listener = {0};
float CSound3D::m_speed_of_sound = X3DAUDIO_SPEED_OF_SOUND;

CSound3D::CSound3D(float CurveDistanceScaler) :
m_calculated(false){
	SecureZeroMemory(&m_emitter, sizeof(m_emitter));
	m_emitter.pCone = nullptr;
	m_emitter.ChannelCount = 1;
	m_emitter.CurveDistanceScaler = CurveDistanceScaler;
	m_emitter.InnerRadius = 0.0f;
	m_emitter.InnerRadiusAngle = 0.0f;

	SecureZeroMemory(&m_DSPSettings, sizeof(m_DSPSettings));
	m_DSPSettings.SrcChannelCount = INPUTCHANNELS;
	m_DSPSettings.DstChannelCount = OUTPUTCHANNELS;
	m_DSPSettings.pMatrixCoefficients = m_matrixOutput;
	m_DSPSettings.pDelayTimes = m_matrixDelay;

	sound3DList.push_back(this);
}

CSound3D::~CSound3D() {
	m_voiceList.clear();
	auto it = sound3DList.begin();
	while (it != sound3DList.end()) {
		if ((*it) == this) {
			it = sound3DList.erase(it);
		} else {
			it++;
		}
	}
}

HRESULT CSound3D::Initialize(float speed_of_sound) {
	X3DAudioInitialize(SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT, speed_of_sound, m_X3DInstance);
	m_speed_of_sound = speed_of_sound;

	m_listener.pCone = nullptr;
	return S_OK;
}

void CSound3D::SetListenerParam(const XMVECTOR& pos, const XMVECTOR& orientFront,
								const XMVECTOR& orientTop, const XMVECTOR& velocity) {
	m_listener.Position.x = pos.m128_f32[0];
	m_listener.Position.y = pos.m128_f32[1];
	m_listener.Position.z = pos.m128_f32[2];

	m_listener.Velocity.x = velocity.m128_f32[0];
	m_listener.Velocity.y = velocity.m128_f32[1];
	m_listener.Velocity.z = velocity.m128_f32[2];

	XMVECTOR frontVec = XMVector3Normalize(orientFront);
	XMVECTOR topVec = XMVector3Normalize(orientTop);
	m_listener.OrientFront.x = frontVec.m128_f32[0];
	m_listener.OrientFront.y = frontVec.m128_f32[1];
	m_listener.OrientFront.z = frontVec.m128_f32[2];

	topVec -= (frontVec * topVec) * frontVec;
	m_listener.OrientTop.x = topVec.m128_f32[0];
	m_listener.OrientTop.y = topVec.m128_f32[1];
	m_listener.OrientTop.z = topVec.m128_f32[2];

	m_calculated = false;
}

void CSound3D::SetListenerCone(X3DAUDIO_CONE* pCone) {
	m_listener.pCone = pCone;
	m_calculated = false;
}

void CSound3D::SetEmitterParam(const XMVECTOR& pos, const XMVECTOR& orientFront,
					 const XMVECTOR& orientTop, const XMVECTOR& velocity) {
	m_emitter.Position.x = pos.m128_f32[0];
	m_emitter.Position.y = pos.m128_f32[1];
	m_emitter.Position.z = pos.m128_f32[2];

	m_emitter.Velocity.x = velocity.m128_f32[0];
	m_emitter.Velocity.y = velocity.m128_f32[1];
	m_emitter.Velocity.z = velocity.m128_f32[2];

	XMVECTOR frontVec = XMVector3Normalize(orientFront);
	XMVECTOR topVec = XMVector3Normalize(orientTop);
	m_emitter.OrientFront.x = frontVec.m128_f32[0];
	m_emitter.OrientFront.y = frontVec.m128_f32[1];
	m_emitter.OrientFront.z = frontVec.m128_f32[2];

	topVec -= (frontVec * topVec) * frontVec;
	m_emitter.OrientTop.x = topVec.m128_f32[0];
	m_emitter.OrientTop.y = topVec.m128_f32[1];
	m_emitter.OrientTop.z = topVec.m128_f32[2];
	m_calculated = false;
}

void CSound3D::SetEmitterCone(X3DAUDIO_CONE* pCone) {
	m_emitter.pCone = pCone;
	m_calculated = false;
}

void CSound3D::SetEmitterInnerRadius(float innerRadius, float angle) {
	m_emitter.InnerRadius = innerRadius;
	m_emitter.InnerRadiusAngle = angle;
	m_calculated = false;
}

void CSound3D::SetEmitterCurveDistanceScaler(float CurveDistanceScaler) {
	m_emitter.CurveDistanceScaler = CurveDistanceScaler;
	m_calculated = false;
}

void CSound3D::calc(UINT flag) {
	if (!m_X3DInstance) {
		return;
	}
	X3DAudioCalculate(m_X3DInstance, &m_listener, &m_emitter, flag, &m_DSPSettings);
	m_calculated = true;
}

void CSound3D::registerVoice(std::shared_ptr<CVoice> voice) const {
	if (voice->IsRunnning() || voice->IsPaused()) {
		m_voiceList.push_back(voice);
	}
}

void CSound3D::unregisterVoice(std::shared_ptr<CVoice> voice) const {
	auto it = m_voiceList.begin();
	while (it != m_voiceList.end()) {
		if (*it == voice) {
			it = m_voiceList.erase(it);
		} else {
			it++;
		}
	}
}

void CSound3D::clearVoice() {
	m_voiceList.clear();
}

void CSound3D::applyVoice(UINT flag) {
	if (!m_calculated && !m_voiceList.empty()) {
		calc(flag);
	}
	auto it = m_voiceList.begin();
	while (it != m_voiceList.end()) {
		if (!(*it)->IsRunnning() && !(*it)->IsPaused()) {
			it = m_voiceList.erase(it);
		} else {
			if ((*it)->IsRunnning()) {
				m_DSPSettings.DopplerFactor = (m_speed_of_sound - m_DSPSettings.ListenerVelocityComponent) 
					/ (m_speed_of_sound - m_DSPSettings.EmitterVelocityComponent);
				(*it)->SetFrequencyRatio(m_DSPSettings.DopplerFactor);
				(*it)->SetOutputMatrix(INPUTCHANNELS, OUTPUTCHANNELS, m_DSPSettings.pMatrixCoefficients);
			}
			it++;
		}
	}
}

void CSound3D::applyVoiceAll(UINT flag) {
	for (auto it = sound3DList.begin(); it != sound3DList.end();it++) {
		(*it)->applyVoice(flag);
	}
}
