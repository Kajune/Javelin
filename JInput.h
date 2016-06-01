#pragma once

#include "JInputKeyboard.h"
#include "JInputMouse.h"
#include "JInputPad.h"

namespace Javelin {
	namespace Input {
		//‰½‚ç‚©‚Ì“ü—Í‚ª‚³‚ê‚Ä‚¢‚é‚©‚Ç‚¤‚©‚ğ•Ô‚·
		static bool IsPressedAny() {
			return InputKeyboard::IsPressedAny() || InputMouse::IsPressedAny();
		}
		//‰½‚ç‚©‚Ì“ü—Í‚ª‚³‚ê‚½uŠÔ‚©‚Ç‚¤‚©‚ğ•Ô‚·
		static bool IsDownAny() {
			return InputKeyboard::IsKeyDownAny() || InputMouse::IsClickedAny();
		}
		//‰½‚ç‚©‚Ì“ü—Í‚ª—£‚³‚ê‚½uŠÔ‚©‚Ç‚¤‚©‚ğ•Ô‚·
		static bool IsUpAny() {
			return InputKeyboard::IsKeyUpAny() || InputMouse::IsReleasedAny();
		}
		static void GetInputState() {
			InputKeyboard::GetKeyboardState();
			InputMouse::GetMouseState();
			InputPad::GetPadState();
		}
	}
}
