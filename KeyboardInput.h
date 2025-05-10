#pragma once
#include "linal.h"

namespace cube_game
{
	class KeyboardInput
	{
	public:
		V3i GetMainInput(bool once = false); // up, down, left, right, wasd
		int GetLogicInpit(bool once = false); // enter, esc
		bool GetModifierInput(bool once = false); // shift, ctrl, alt

		static const V3i up;
		static const V3i down;
		static const V3i left;
		static const V3i right;
		static const V3i yaw_left;
		static const V3i jaw_right;
		static const V3i zero;
		static const int yes;
		static const int no;
	};
}
