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

		//初期化を行う。speed_of_soundにユーザー定義のワールド単位/秒で速度を指定する。
		//この関数は必ず呼び出す必要がある。
		static HRESULT Initialize(float speed_of_sound = X3DAUDIO_SPEED_OF_SOUND);
		//リスナーに関するパラメータを指定する
		void SetListenerParam(const XMVECTOR& pos, const XMVECTOR& orientFront,
			const XMVECTOR& orientTop = XMLoadFloat3(&XMFLOAT3(0, 1, 0)),
			const XMVECTOR& velocity = XMLoadFloat3(&XMFLOAT3(0, 0, 0)));
		//リスナーコーンを指定する
		void SetListenerCone(X3DAUDIO_CONE* pCone);
		//エミッターに関するパラメータを指定する
		void SetEmitterParam(const XMVECTOR& pos, const XMVECTOR& orientFront,
			const XMVECTOR& orientTop = XMLoadFloat3(&XMFLOAT3(0, 0, 0)),
			const XMVECTOR& velocity = XMLoadFloat3(&XMFLOAT3(0, 0, 0)));
		//エミッターコーンを指定する
		void SetEmitterCone(X3DAUDIO_CONE* pCone);
		//エミッターの内部半径を指定する
		void SetEmitterInnerRadius(float innerRadius, float angle);
		//エミッターのカーブ距離スケーラを指定する
		void SetEmitterCurveDistanceScaler(float CurveDistanceScaler);
		//指定されたパラメータを元に計算を行う
		void calc(UINT flag = kDefaultFlag);
		//計算結果を適用するボイスを登録する
		void registerVoice(std::shared_ptr<CVoice> voice) const;
		//ボイスを計算結果適用から除外する（注意：時間がかかる恐れがあります）
		//再生終了したボイスは自動的に除外されるため、この関数を呼ぶ必要がありません
		void unregisterVoice(std::shared_ptr<CVoice> voice) const;
		//登録されているボイスを全てクリアする
		void clearVoice();
		//登録されているボイスに計算結果を適用する
		void applyVoice(UINT flag = kDefaultFlag);
		//全ての登録されているボイスに計算結果を適用する
		static void applyVoiceAll(UINT flag = kDefaultFlag);
	};

}