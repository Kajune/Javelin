#pragma once

#include "JGlobal.h"
#include <array>
#include <functional>

namespace Javelin {

	class InputKeyboard : private UnConstructable {
		friend class Application;

		static constexpr UINT kMaxKey = 0xF0;
		typedef struct {
			bool isPressed;
			bool isToggled;
			int pressedTime;
		}keyState_t;
		static std::array<keyState_t, kMaxKey>	m_keyState;
		static void GetKeyboardState();

		static bool IsAny(std::function<bool(UINT)> cond, UINT* key) {
			for (UINT i = 0; i < m_keyState.size(); i++) {
				if (cond(i)) {
					if (key) {
						*key = i;
					}
					return true;
				}
			}
		}
	public:
		//key��������Ă��邩�ǂ�����Ԃ�
		static bool IsPressed(UINT key) {
			return key < kMaxKey && m_keyState.at(key).isPressed;
		}
		//key���g�O������Ă��邩�ǂ�����Ԃ�
		static bool IsToggled(UINT key) {
			return key < kMaxKey && m_keyState.at(key).isToggled;
		}
		//key�̉�����Ă��鎞�Ԃ�Ԃ�
		static int GetPressedTime(UINT key) {
			return key < kMaxKey ? m_keyState.at(key).pressedTime : 0;
		}
		//key�������ꂽ�u�Ԃ��ǂ�����Ԃ�
		static bool IsKeyDown(UINT key) {
			return key < kMaxKey && m_keyState.at(key).isPressed && m_keyState.at(key).pressedTime == 0;
		}
		//key�������ꂽ�u�Ԃ��ǂ�����Ԃ�
		static bool IsKeyUp(UINT key) {
			return key < kMaxKey && !m_keyState.at(key).isPressed && m_keyState.at(key).pressedTime > 0;
		}
		//�����ꂩ�̃L�[��������Ă��邩�ǂ�����Ԃ�
		static bool IsPressedAny(UINT* key = nullptr) {
			return IsAny(IsPressed, key);
		}
		//�����ꂩ�̃L�[���g�O������Ă��邩�ǂ�����Ԃ�
		static bool IsToggledAny(UINT* key = nullptr) {
			return IsAny(IsToggled, key);
		}
		//�����ꂩ�̃L�[�������ꂽ�u�Ԃ��ǂ�����Ԃ�
		static bool IsKeyDownAny(UINT* key = nullptr) {
			return IsAny(IsKeyDown, key);
		}
		//�����ꂩ�̃L�[�������ꂽ�u�Ԃ��ǂ�����Ԃ�
		static bool IsKeyUpAny(UINT* key = nullptr) {
			return IsAny(IsKeyUp, key);
		}
		//key�̖��O��Ԃ�
		static std::string GetKeyName(UINT key);
	};

}
