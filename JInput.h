#pragma once

#include "JInputKeyboard.h"
#include "JInputMouse.h"
#include "JInputPad.h"

namespace Javelin {
	namespace Input {
		//何らかの入力がされているかどうかを返す
		static bool IsPressedAny() {
			return InputKeyboard::IsPressedAny() || InputMouse::IsPressedAny() || InputPad::IsPressedAny();
		}
		//何らかの入力がされた瞬間かどうかを返す
		static bool IsDownAny() {
			return InputKeyboard::IsKeyDownAny() || InputMouse::IsClickedAny() || InputPad::IsDownAny();
		}
		//何らかの入力が離された瞬間かどうかを返す
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
