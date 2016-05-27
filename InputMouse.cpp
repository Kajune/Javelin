#include "JInputMouse.h"

using namespace Javelin;

UINT InputMouse::button[3] = {0, 0, 0};
int InputMouse::wheelRotation = 0;
bool InputMouse::wheelRotated = false;
bool InputMouse::relativeMode = false;
int
InputMouse::x = 0, InputMouse::y = 0,
InputMouse::rx = 0, InputMouse::ry = 0,
InputMouse::wx = 0, InputMouse::wy = 0,
InputMouse::cx = 0, InputMouse::cy = 0;

void InputMouse::GetMouseState() {
	POINT point;
	GetCursorPos(&point);

	rx = point.x - x;
	ry = point.y - y;
	x = point.x;
	y = point.y;

	RECT rc;
	GetWindowRect(Application::GetHWnd(), &rc);

	if (relativeMode) {
		x = (rc.right + rc.left) / 2;
		y = (rc.bottom + rc.top) / 2;
		SetCursorPos(x, y);
	}

	wx = x - rc.left;
	wy = y - rc.right;

	ScreenToClient(Application::GetHWnd(), &point);
	cx = point.x;
	cy = point.y;

	for (auto& it : button) {
		if (it > 0) {
			it--;
		}
	}

	if (wheelRotated) {
		wheelRotation = 0;
		wheelRotated = false;
	}
	if (wheelRotation != 0) {
		wheelRotated = true;
	}
}

void InputMouse::DoubleClicked(UINT index) {
	button[index] = 2;
}

void InputMouse::WheelRotated(int mass) {
	wheelRotation = mass;
}
