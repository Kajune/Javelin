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
		//keyが押されているかどうかを返す
		static bool IsPressed(UINT key) {
			return key < kMaxKey && m_keyState.at(key).isPressed;
		}
		//keyがトグルされているかどうかを返す
		static bool IsToggled(UINT key) {
			return key < kMaxKey && m_keyState.at(key).isToggled;
		}
		//keyの押されている時間を返す
		static int GetPressedTime(UINT key) {
			return key < kMaxKey ? m_keyState.at(key).pressedTime : 0;
		}
		//keyが押された瞬間かどうかを返す
		static bool IsKeyDown(UINT key) {
			return key < kMaxKey && m_keyState.at(key).isPressed && m_keyState.at(key).pressedTime == 0;
		}
		//keyが離された瞬間かどうかを返す
		static bool IsKeyUp(UINT key) {
			return key < kMaxKey && !m_keyState.at(key).isPressed && m_keyState.at(key).pressedTime > 0;
		}
		//いずれかのキーが押されているかどうかを返す
		static bool IsPressedAny(UINT* key = nullptr) {
			return IsAny(IsPressed, key);
		}
		//いずれかのキーがトグルされているかどうかを返す
		static bool IsToggledAny(UINT* key = nullptr) {
			return IsAny(IsToggled, key);
		}
		//いずれかのキーが押された瞬間かどうかを返す
		static bool IsKeyDownAny(UINT* key = nullptr) {
			return IsAny(IsKeyDown, key);
		}
		//いずれかのキーが離された瞬間かどうかを返す
		static bool IsKeyUpAny(UINT* key = nullptr) {
			return IsAny(IsKeyUp, key);
		}
		//keyの名前を返す
		static std::string GetKeyName(UINT key);
	};

}
