#include "JInputKeyboard.h"

using namespace Javelin;

std::array<InputKeyboard::keyState_t, InputKeyboard::kMaxKey>	InputKeyboard::m_keyState;

void InputKeyboard::GetKeyboardState() {
	for (auto& it : m_keyState) {
		if (it.isPressed) {
			it.pressedTime++;
		} else {
			it.pressedTime = 0;
		}
	}

	BYTE state[256];
	::GetKeyboardState(state);

	for (UINT i = 0; i < kMaxKey; i++) {
		m_keyState.at(i).isPressed = (state[i] & 0x80) > 0;
		m_keyState.at(i).isToggled = (state[i] & 0x81) > 0;
	}
}

std::string InputKeyboard::GetKeyName(UINT key) {
	if (key >= kMaxKey) {
		return "";
	}
	constexpr int size = 32;
	char buf[size];
	GetKeyNameText((LONG)(MapVirtualKey(key, MAPVK_VK_TO_VSC) << 16), buf, size);
	return buf;
}
