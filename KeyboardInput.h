#pragma once
#include "linal.h"

namespace cube_game
{
	class KeyboardInput
	{
	public:
		V2f GetMainInput(bool once = true); // up, down, left, right, wasd
		float GetLogicInpit(bool once = true); // enter, esc
		bool GetModifierInput(bool once = false); // shift, ctrl, alt

		static const V2f up;
		static const V2f down;
		static const V2f left;
		static const V2f right;
		static const V2f zero;
		static const float yes;
		static const float no;
	};
}
