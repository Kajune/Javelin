#include "JInputPad.h"
#include "JApplication.h"

using namespace Javelin;

InputPad::pad_state_t InputPad::pad_state[kMaxPadNum];

namespace {
	UINT flags[InputPad::kMaxButtonNum] = {
		JOY_BUTTON1,
		JOY_BUTTON2,
		JOY_BUTTON3,
		JOY_BUTTON4,
		JOY_BUTTON5,
		JOY_BUTTON6,
		JOY_BUTTON7,
		JOY_BUTTON8,
		JOY_BUTTON9,
		JOY_BUTTON10,
		JOY_BUTTON11,
		JOY_BUTTON12,
		JOY_BUTTON13,
		JOY_BUTTON14,
		JOY_BUTTON15,
		JOY_BUTTON16,
		JOY_BUTTON17,
		JOY_BUTTON18,
		JOY_BUTTON19,
		JOY_BUTTON20,
		JOY_BUTTON21,
		JOY_BUTTON22,
		JOY_BUTTON23,
		JOY_BUTTON24,
		JOY_BUTTON25,
		JOY_BUTTON26,
		JOY_BUTTON27,
		JOY_BUTTON28,
		JOY_BUTTON29,
		JOY_BUTTON30,
		JOY_BUTTON31,
		JOY_BUTTON32,
	};
}

void InputPad::GetPadState() {
	for (UINT i = 0; i < kMaxPadNum; i++) {
		JOYCAPS caps{};
		auto ret = joyGetDevCaps(i, &caps, sizeof(caps));
		if (ret != JOYERR_NOERROR) {
			pad_state[i].connected = false;
			continue;
		}
		JOYINFOEX info;
		info.dwSize = sizeof(info);
		info.dwFlags = JOY_RETURNALL;
		ret = joyGetPosEx(i, &info);
		if (ret == MMSYSERR_NODRIVER) {
			pad_state[i].connected = false;
			continue;
		} else if (ret == JOYERR_NOERROR) {
			pad_state[i].connected = true;
		} else {
			pad_state[i].connected = false;
			continue;
		}

		for (UINT j = 0; j < kMaxButtonNum; j++) {
			if (pad_state[i].pressedButton[j]) {
				pad_state[i].pressedTimeButton[j]++;
			} else {
				pad_state[i].pressedTimeButton[j] = 0;
			}
			pad_state[i].pressedButton[j] = (info.dwButtons & flags[j]) > 0;
		}

		pad_state[i].povAngle = XMConvertToRadians((float)info.dwPOV / 100.0f);
		for (UINT j = 0; j < kMaxPovNum; j++) {
			if (pad_state[i].pressedPov[j]) {
				pad_state[i].pressedTimePov[j]++;
			} else {
				pad_state[i].pressedTimePov[j] = 0;
			}
			pad_state[i].pressedPov[j] = info.dwPOV <= 36000 &&
				(fabsf(pad_state[i].povAngle - J_PI * 2.0f * float(j) / (float)kMaxPovNum)
					<= J_PI * 1.01f / (float)kMaxPovNum
					|| fabsf(pad_state[i].povAngle - J_PI * 2.0f * (1.0f + float(j) / (float)kMaxPovNum))
					<= J_PI * 1.01f / (float)kMaxPovNum);
		}

		for (UINT j = 0; j < kMaxStickNum; j++) {
			if (fabsf(pad_state[i].stickValue[j]) >= pad_state[i].threshold) {
				pad_state[i].stickTime[j]++;
			} else {
				pad_state[i].stickTime[j] = 0;
			}
		}

		pad_state[i].stickValue[0] = float(info.dwXpos) / float(kMaxStickValue) - 1.0f;
		pad_state[i].stickValue[1] = float(info.dwYpos) / float(kMaxStickValue) - 1.0f;
		pad_state[i].stickValue[2] = float(info.dwZpos) / float(kMaxStickValue) - 1.0f;
		pad_state[i].stickValue[3] = float(info.dwRpos) / float(kMaxStickValue) - 1.0f;
		pad_state[i].stickValue[4] = float(info.dwUpos) / float(kMaxStickValue) - 1.0f;
		pad_state[i].stickValue[5] = float(info.dwVpos) / float(kMaxStickValue) - 1.0f;
	}
}

