#pragma once
#include "JSound.h"
#include "X3DAudio.h"
#include "JGlobal.h"

namespace Javelin {

	class CSound3D {
		static const UINT INPUTCHANNELS = 1;
		static const UINT OUTPUTCHANNELS = 2;

		static const UINT kDefaultFlag = X3DAUDIO_CALCULATE_MATRIX
			| X3DAUDIO_CALCULATE_DOPPLER
			| X3DAUDIO_CALCULATE_EMITTER_ANGLE
			| X3DAUDIO_CALCULATE_DELAY;

		static X3DAUDIO_HANDLE	m_X3DInstance;
		static X3DAUDIO_LISTENER	m_listener;
		static float			m_speed_of_sound;
		X3DAUDIO_EMITTER		m_emitter;
		X3DAUDIO_DSP_SETTINGS	m_DSPSettings;
		FLOAT32					m_matrixOutput[OUTPUTCHANNELS];
		FLOAT32					m_matrixDelay[OUTPUTCHANNELS];
		bool					m_calculated;

		mutable std::list<std::shared_ptr<CVoice>> m_voiceList;
		static std::list<CSound3D*> sound3DList;
	public:
		CSound3D(float CurveDistanceScaler = 1.0f);
		~CSound3D();

		//���������s���Bspeed_of_sound�Ƀ��[�U�[��`�̃��[���h�P��/�b�ő��x���w�肷��B
		//���̊֐��͕K���Ăяo���K�v������B
		static HRESULT Initialize(float speed_of_sound = X3DAUDIO_SPEED_OF_SOUND);
		//���X�i�[�Ɋւ���p�����[�^���w�肷��
		void SetListenerParam(const XMVECTOR& pos, const XMVECTOR& orientFront,
			const XMVECTOR& orientTop = XMLoadFloat3(&XMFLOAT3(0, 1, 0)),
			const XMVECTOR& velocity = XMLoadFloat3(&XMFLOAT3(0, 0, 0)));
		//���X�i�[�R�[�����w�肷��
		void SetListenerCone(X3DAUDIO_CONE* pCone);
		//�G�~�b�^�[�Ɋւ���p�����[�^���w�肷��
		void SetEmitterParam(const XMVECTOR& pos, const XMVECTOR& orientFront,
			const XMVECTOR& orientTop = XMLoadFloat3(&XMFLOAT3(0, 0, 0)),
			const XMVECTOR& velocity = XMLoadFloat3(&XMFLOAT3(0, 0, 0)));
		//�G�~�b�^�[�R�[�����w�肷��
		void SetEmitterCone(X3DAUDIO_CONE* pCone);
		//�G�~�b�^�[�̓������a���w�肷��
		void SetEmitterInnerRadius(float innerRadius, float angle);
		//�G�~�b�^�[�̃J�[�u�����X�P�[�����w�肷��
		void SetEmitterCurveDistanceScaler(float CurveDistanceScaler);
		//�w�肳�ꂽ�p�����[�^�����Ɍv�Z���s��
		void calc(UINT flag = kDefaultFlag);
		//�v�Z���ʂ�K�p����{�C�X��o�^����
		void registerVoice(std::shared_ptr<CVoice> voice) const;
		//�{�C�X���v�Z���ʓK�p���珜�O����i���ӁF���Ԃ������鋰�ꂪ����܂��j
		//�Đ��I�������{�C�X�͎����I�ɏ��O����邽�߁A���̊֐����ĂԕK�v������܂���
		void unregisterVoice(std::shared_ptr<CVoice> voice) const;
		//�o�^����Ă���{�C�X��S�ăN���A����
		void clearVoice();
		//�o�^����Ă���{�C�X�Ɍv�Z���ʂ�K�p����
		void applyVoice(UINT flag = kDefaultFlag);
		//�S�Ă̓o�^����Ă���{�C�X�Ɍv�Z���ʂ�K�p����
		static void applyVoiceAll(UINT flag = kDefaultFlag);
	};

}