#include "JInputMouse.h"

using namespace Javelin;

UINT InputMouse::button[3] = {0, 0, 0};
int InputMouse::wheelRotation = 0;
bool InputMouse::wheelRotated = false;
bool InputMouse::relativeMode = false;
float 
InputMouse::x = 0.0f, InputMouse::y = 0.0f,
InputMouse::rx = 0.0f, InputMouse::ry = 0.0f,
InputMouse::wx = 0.0f, InputMouse::wy = 0.0f,
InputMouse::cx = 0.0f, InputMouse::cy = 0.0f;

void InputMouse::GetMouseState() {
	POINT point;
	GetCursorPos(&point);

	rx = (float)point.x - x;
	ry = (float)point.y - y;
	x = (float)point.x;
	y = (float)point.y;

	RECT rc;
	GetWindowRect(Application::GetHWnd(), &rc);

	if (relativeMode) {
		x = (float)(rc.right + rc.left) / 2.0f;
		y = (float)(rc.bottom + rc.top) / 2.0f;
		SetCursorPos((int)x, (int)y);
	}

	wx = x - (float)rc.left;
	wy = y - (float)rc.right;

	ScreenToClient(Application::GetHWnd(), &point);
	cx = (float)point.x;
	cy = (float)point.y;

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
