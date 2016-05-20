#pragma once

#include "JGlobal.h"
#include "XAudio2.h"
#include <memory>

namespace Javelin {

	class CSound;
	class CVoice;
	class CSound3D;

	class SoundPlayer : private UnConstructable {
		friend class Application;

		static IXAudio2*					m_pXAudio2;
		static IXAudio2MasteringVoice*		m_pMasterVoice;
		static XAUDIO2_DEVICE_DETAILS		m_details;

		static void Initialize();
		static void Cleanup() noexcept;
	public:
		//XAUDIO2_BUFFER�𒼐ڎw�肵�čĐ�����
		static void Play(const CSound& sound, const XAUDIO2_BUFFER* buffer,
			std::shared_ptr<CVoice>* ppVoice, const CSound3D* pSound3D);
		//1�񂾂����ʂɍĐ�����
		static void PlayNormal(const CSound& sound,
			std::shared_ptr<CVoice>* ppVoice = nullptr, const CSound3D* pSound3D = nullptr);
		//�w�肵���񐔃��[�v�Đ�����@�񐔂ɕ��̒l���w�肷��Ɩ������[�v�ł���
		static void PlayLoop(const CSound& sound, int loopTimes = -1,
			std::shared_ptr<CVoice>* ppVoice = nullptr, const CSound3D* pSound3D = nullptr);
		//�Đ����ꎞ��~����
		static void Pause(const CSound& sound);
		//�ꎞ��~�����Đ����ĊJ����
		static void Resume(const CSound& sound);
		//�Đ����~����
		static void Stop(const CSound& sound);
		//�}�X�^���ʂ�ݒ肷��
		static void SetVolume(float volume);
		//�}�X�^���ʂ𓾂�
		static float GetVolume();
		//�f�o�C�X�̏ڍׂ𓾂�
		static const XAUDIO2_DEVICE_DETAILS& GetDetail() {
			return m_details;
		}
	};

}
