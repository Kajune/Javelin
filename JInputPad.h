#pragma once

#include "JGlobal.h"
#include <functional>

namespace Javelin {

	class InputPad : private UnConstructable {
	public:
		static const UINT kMaxPadNum = 2;
		static const UINT kMaxButtonNum = 32;
		static const UINT kMaxPovNum = 4;
		static const UINT kMaxStickNum = 6;
		enum POV_DIRECTION : UINT {
			UP = 0,
			DOWN = 2,
			LEFT = 1,
			RIGHT = 3,
		};
	private:
		typedef struct {
			bool connected;

			bool pressedButton[kMaxButtonNum];
			UINT pressedTimeButton[kMaxButtonNum];

			bool pressedPov[kMaxPovNum];
			UINT pressedTimePov[kMaxPovNum];
			float povAngle;

			float stickValue[kMaxStickNum];
			UINT stickTime[kMaxStickNum];
			float threshold = 0.01f;
		}pad_state_t;
		static pad_state_t pad_state[kMaxPadNum];

		static const UINT kMaxStickValue = 0x7fff;

		static bool checkAll(UINT maxNum, std::function<bool(UINT)> cond) {
			for (UINT i = 0; i < maxNum; i++) {
				if (cond(i)) {
					return true;
				}
			}
			return false;
		}
	public:
		static void GetPadState();

		static bool IsConnected(UINT padNum = 0) noexcept {
			return padNum < kMaxButtonNum && pad_state[padNum].connected;
		}

		static bool IsButtonPressed(UINT buttonNum, UINT padNum = 0) noexcept {
			return IsConnected(padNum) && buttonNum < kMaxButtonNum 
				&& pad_state[padNum].pressedButton[buttonNum];
		}
		static UINT GetButtonPressedTime(UINT buttonNum, UINT padNum = 0) noexcept {
			return IsConnected(padNum) && buttonNum < kMaxButtonNum ? 
				pad_state[padNum].pressedTimeButton[buttonNum] : 0;
		}
		static bool IsButtonDown(UINT buttonNum, UINT padNum = 0) noexcept {
			return IsButtonPressed(buttonNum, padNum) && GetButtonPressedTime(buttonNum, padNum) == 0;
		}
		static bool IsButtonUp(UINT buttonNum, UINT padNum = 0) noexcept {
			return !IsButtonPressed(buttonNum, padNum) && GetButtonPressedTime(buttonNum, padNum) > 0;
		}
		static bool IsAnyButtonPressed(UINT padNum = 0) noexcept {
			return checkAll(kMaxButtonNum, std::bind(IsButtonPressed, std::placeholders::_1, padNum));
		}
		static bool IsAnyButtonDown(UINT padNum = 0) noexcept {
			return checkAll(kMaxButtonNum, std::bind(IsButtonDown, std::placeholders::_1, padNum));
		}
		static bool IsAnyButtonUp(UINT padNum = 0) noexcept {
			return checkAll(kMaxButtonNum, std::bind(IsButtonUp, std::placeholders::_1, padNum));
		}
		static bool IsAnyButtonPressedAllPads() noexcept {
			return checkAll(kMaxPadNum, IsAnyButtonPressed);
		}
		static bool IsAnyButtonDownAllPads() noexcept {
			return checkAll(kMaxPadNum, IsAnyButtonDown);
		}
		static bool IsAnyButtonUpAllPads() noexcept {
			return checkAll(kMaxPadNum, IsAnyButtonUp);
		}

		static bool IsPovPressed(UINT PovNum, UINT padNum = 0) noexcept {
			return IsConnected(padNum) && PovNum < kMaxPovNum
				&& pad_state[padNum].pressedPov[PovNum];
		}
		static UINT GetPovPressedTime(UINT PovNum, UINT padNum = 0) noexcept {
			return IsConnected(padNum) && PovNum < kMaxPovNum ?
				pad_state[padNum].pressedTimePov[PovNum] : 0;
		}
		static bool IsPovDown(UINT PovNum, UINT padNum = 0) noexcept {
			return IsPovPressed(PovNum, padNum) && GetPovPressedTime(PovNum, padNum) == 0;
		}
		static bool IsPovUp(UINT PovNum, UINT padNum = 0) noexcept {
			return !IsPovPressed(PovNum, padNum) && GetPovPressedTime(PovNum, padNum) > 0;
		}
		static bool IsAnyPovPressed(UINT padNum = 0) noexcept {
			return checkAll(kMaxPovNum, std::bind(IsPovPressed, std::placeholders::_1, padNum));
		}
		static bool IsAnyPovDown(UINT padNum = 0) noexcept {
			return checkAll(kMaxPovNum, std::bind(IsPovDown, std::placeholders::_1, padNum));
		}
		static bool IsAnyPovUp(UINT padNum = 0) noexcept {
			return checkAll(kMaxPovNum, std::bind(IsPovUp, std::placeholders::_1, padNum));
		}
		static bool IsAnyPovPressedAllPads() noexcept {
			return checkAll(kMaxPadNum, IsAnyPovPressed);
		}
		static bool IsAnyPovDownAllPads() noexcept {
			return checkAll(kMaxPadNum, IsAnyPovDown);
		}
		static bool IsAnyPovUpAllPads() noexcept {
			return checkAll(kMaxPadNum, IsAnyPovUp);
		}
		static float GetPovAngle(UINT padNum = 0) noexcept {
			return IsConnected(padNum) ? pad_state[padNum].povAngle : 0.0f;
		}

		static float GetStickValue(UINT stickNum, UINT padNum = 0) noexcept {
			return IsConnected(padNum) && stickNum < kMaxStickNum 
				&& fabsf(pad_state[padNum].stickValue[stickNum]) >= pad_state[padNum].threshold ?
				pad_state[padNum].stickValue[stickNum] : 0.0f;
		}
		static float GetStickLX(UINT padNum = 0) noexcept {
			return GetStickValue(0, padNum);
		}
		static float GetStickLY(UINT padNum = 0) noexcept {
			return GetStickValue(1, padNum);
		}
		static float GetStickRX(UINT padNum = 0) noexcept {
			return GetStickValue(2, padNum);
		}
		static float GetStickRY(UINT padNum = 0) noexcept {
			return GetStickValue(3, padNum);
		}
		static float GetStickLRotation(UINT padNum = 0) noexcept {
			return GetStickValue(4, padNum);
		}
		static float GetStickRRotation(UINT padNum = 0) noexcept {
			return GetStickValue(5, padNum);
		}
		static bool IsStickPressed(UINT stickNum, UINT padNum = 0) noexcept {
			return fabsf(GetStickValue(stickNum, padNum)) >= pad_state[padNum].threshold / 2.0f;
		}
		static bool IsStickPressedLX(UINT padNum = 0) noexcept {
			return IsStickPressed(0, padNum);
		}
		static bool IsStickPressedLY(UINT padNum = 0) noexcept {
			return IsStickPressed(1, padNum);
		}
		static bool IsStickPressedRX(UINT padNum = 0) noexcept {
			return IsStickPressed(2, padNum);
		}
		static bool IsStickPressedRY(UINT padNum = 0) noexcept {
			return IsStickPressed(3, padNum);
		}
		static bool IsStickPressedLRotation(UINT padNum = 0) noexcept {
			return IsStickPressed(4, padNum);
		}
		static bool IsStickPressedRRotation(UINT padNum = 0) noexcept {
			return IsStickPressed(5, padNum);
		}
		static UINT GetStickTime(UINT stickNum, UINT padNum = 0) noexcept {
			return IsConnected(padNum) && stickNum < kMaxStickNum ?
				pad_state[padNum].stickTime[stickNum] : 0;
		}
		static UINT GetStickLXTime(UINT padNum = 0) noexcept {
			return GetStickTime(0, padNum);
		}
		static UINT GetStickLYTime(UINT padNum = 0) noexcept {
			return GetStickTime(1, padNum);
		}
		static UINT GetStickRXTime(UINT padNum = 0) noexcept {
			return GetStickTime(2, padNum);
		}
		static UINT GetStickRYTime(UINT padNum = 0) noexcept {
			return GetStickTime(3, padNum);
		}
		static UINT GetStickLRotationTime(UINT padNum = 0) noexcept {
			return GetStickTime(4, padNum);
		}
		static UINT GetStickRRotationTime(UINT padNum = 0) noexcept {
			return GetStickTime(5, padNum);
		}
		static bool IsStickDown(UINT stickNum, UINT padNum = 0) noexcept {
			return IsStickPressed(stickNum) && GetStickTime(stickNum, padNum) == 0;
		}
		static bool IsStickDownLX(UINT padNum = 0) noexcept {
			return IsStickDown(0, padNum);
		}
		static bool IsStickDownLY(UINT padNum = 0) noexcept {
			return IsStickDown(1, padNum);
		}
		static bool IsStickDownRX(UINT padNum = 0) noexcept {
			return IsStickDown(2, padNum);
		}
		static bool IsStickDownRY(UINT padNum = 0) noexcept {
			return IsStickDown(3, padNum);
		}
		static bool IsStickDownLRotation(UINT padNum = 0) noexcept {
			return IsStickDown(4, padNum);
		}
		static bool IsStickDownRRotation(UINT padNum = 0) noexcept {
			return IsStickDown(5, padNum);
		}
		static bool IsStickUp(UINT stickNum, UINT padNum = 0) noexcept {
			return !IsStickPressed(stickNum) && GetStickTime(stickNum, padNum) > 0;
		}
		static bool IsStickUpLX(UINT padNum = 0) noexcept {
			return IsStickUp(0, padNum);
		}
		static bool IsStickUpLY(UINT padNum = 0) noexcept {
			return IsStickUp(1, padNum);
		}
		static bool IsStickUpRX(UINT padNum = 0) noexcept {
			return IsStickUp(2, padNum);
		}
		static bool IsStickUpRY(UINT padNum = 0) noexcept {
			return IsStickUp(3, padNum);
		}
		static bool IsStickUpLRotation(UINT padNum = 0) noexcept {
			return IsStickUp(4, padNum);
		}
		static bool IsStickUpRRotation(UINT padNum = 0) noexcept {
			return IsStickUp(5, padNum);
		}
		static bool IsAnyStickPressed(UINT padNum = 0) noexcept {
			return checkAll(kMaxStickNum, std::bind(IsStickPressed, std::placeholders::_1, padNum));
		}
		static bool IsAnyStickDown(UINT padNum = 0) noexcept {
			return checkAll(kMaxStickNum, std::bind(IsStickDown, std::placeholders::_1, padNum));
		}
		static bool IsAnyStickUp(UINT padNum = 0) noexcept {
			return checkAll(kMaxStickNum, std::bind(IsStickUp, std::placeholders::_1, padNum));
		}
		static bool IsAnyStickPressedAllPads() noexcept {
			return checkAll(kMaxPadNum, IsAnyStickPressed);
		}
		static bool IsAnyStickDownAllPads() noexcept {
			return checkAll(kMaxPadNum, IsAnyStickDown);
		}
		static bool IsAnyStickUpAllPads() noexcept {
			return checkAll(kMaxPadNum, IsAnyStickUp);
		}

		static bool IsPressedAny() noexcept {
			return IsAnyButtonPressedAllPads() || IsAnyPovPressedAllPads() || IsAnyStickPressedAllPads();
		}
		static bool IsDownAny() noexcept {
			return IsAnyButtonDownAllPads() || IsAnyPovDownAllPads() || IsAnyStickDownAllPads();
		}
		static bool IsUpAny() noexcept {
			return IsAnyButtonUpAllPads() || IsAnyPovUpAllPads() || IsAnyStickUpAllPads();
		}

		//ジョイスティックが反応する閾値を0.0f~1.0fの範囲で指定する
		//初期値は0.01
		static void SetThreshold(float threshold, UINT padNum = 0) noexcept {
			if (padNum < kMaxPadNum) {
				pad_state[padNum].threshold = threshold;
			}
		}
	};

}
