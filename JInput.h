#pragma once

#include "JInputKeyboard.h"
#include "JInputMouse.h"
#include "JInputPad.h"

namespace Javelin {
	namespace Input {
		//���炩�̓��͂�����Ă��邩�ǂ�����Ԃ�
		static bool IsPressedAny() {
			return InputKeyboard::IsPressedAny() || InputMouse::IsPressedAny() || InputPad::IsPressedAny();
		}
		//���炩�̓��͂����ꂽ�u�Ԃ��ǂ�����Ԃ�
		static bool IsDownAny() {
			return InputKeyboard::IsKeyDownAny() || InputMouse::IsClickedAny() || InputPad::IsDownAny();
		}
		//���炩�̓��͂������ꂽ�u�Ԃ��ǂ�����Ԃ�
		static bool IsUpAny() {
			return InputKeyboard::IsKeyUpAny() || InputMouse::IsReleasedAny() || InputPad::IsUpAny();
		}
		static void GetInputState() {
			InputKeyboard::GetKeyboardState();
			InputMouse::GetMouseState();
			InputPad::GetPadState();
		}
	}
}
