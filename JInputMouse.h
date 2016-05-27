#pragma once

#include "JGlobal.h"
#include "JApplication.h"
#include "JInputKeyboard.h"

namespace Javelin {

	class InputMouse : private UnConstructable {
		friend class Application;
		enum BUTTONNAME : UINT {
			LEFT = 0,
			RIGHT,
			MIDDLE,
		};

		static UINT button[3];
		static int wheelRotation;
		static bool wheelRotated;
		static bool relativeMode;
		static int x, y, rx, ry, wx, wy, cx, cy;

		static void GetMouseState();
		static void DoubleClicked(UINT index);
		static void WheelRotated(int mass);
	public:
		static int GetMousePosX() {
			return x;
		}
		static int GetMousePosY() {
			return y;
		}
		static int GetRelativeMousePosX() {
			return rx;
		}
		static int GetRelativeMousePosY() {
			return ry;
		}
		static int GetWindowMousePosX() {
			return wx;
		}
		static int GetWindowMousePosY() {
			return wy;
		}
		static int GetClientMousePosX() {
			return cx;
		}
		static int GetClientMousePosY() {
			return cy;
		}

		static bool IsPressedLeft() {
			return InputKeyboard::IsPressed(VK_LBUTTON);
		}
		static bool IsPressedRight() {
			return InputKeyboard::IsPressed(VK_RBUTTON);
		}
		static bool IsPressedMiddle() {
			return InputKeyboard::IsPressed(VK_MBUTTON);
		}
		static int GetPressedTimeLeft() {
			return InputKeyboard::GetPressedTime(VK_LBUTTON);
		}
		static int GetPressedTimeRight() {
			return InputKeyboard::GetPressedTime(VK_RBUTTON);
		}
		static int GetPressedTimeMiddle() {
			return InputKeyboard::GetPressedTime(VK_MBUTTON);
		}
		static bool IsClickedLeft() {
			return InputKeyboard::IsKeyDown(VK_LBUTTON);
		}
		static bool IsClickedRight() {
			return InputKeyboard::IsKeyDown(VK_RBUTTON);
		}
		static bool IsClickedMiddle() {
			return InputKeyboard::IsKeyDown(VK_MBUTTON);
		}
		static bool IsReleasedLeft() {
			return InputKeyboard::IsKeyUp(VK_LBUTTON);
		}
		static bool IsReleasedRight() {
			return InputKeyboard::IsKeyUp(VK_RBUTTON);
		}
		static bool IsReleasedMiddle() {
			return InputKeyboard::IsKeyUp(VK_MBUTTON);
		}
		static bool IsDoubleClickedLeft() {
			return button[LEFT] > 0;
		}
		static bool IsDoubleClickedRight() {
			return button[RIGHT] > 0;
		}
		static bool IsDoubleClickedMiddle() {
			return button[MIDDLE] > 0;
		}
		static bool IsWheelRotated() {
			return wheelRotated;
		}
		static int GetWheelRotation() {
			return wheelRotation;
		}
		static bool IsPressedAny() {
			return IsPressedLeft() || IsPressedRight() || IsPressedMiddle();
		}
		static bool IsClickedAny() {
			return IsClickedLeft() || IsClickedRight() || IsClickedMiddle();
		}
		static bool IsReleasedAny() {
			return IsReleasedLeft() || IsReleasedRight() || IsReleasedMiddle();
		}
		static bool IsDoubleClickedAny() {
			return IsDoubleClickedLeft() || IsDoubleClickedRight() || IsDoubleClickedMiddle();
		}

		//マウスの相対移動モードを指定する
		//相対移動モードになるとカーソルが画面中央に固定され、相対座標のみが取得可能になる
		static void SetRelativeMode(bool flag) {
			relativeMode = flag;
		}
		//カーソルを表示するかどうかを指定する
		static void SetShowCursor(bool show) {
			ShowCursor(show);
		}
	};

}
