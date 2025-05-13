#pragma once
#include "linal.h"

namespace cube_game
{
	class KeyboardInput
	{
	public:
		V3i GetMainInput(bool once = false); // reads WASDQE and returns 3d vector as input.
		int GetLogicInpit(bool once = false); // reads ender and escape and returns int as input value
		bool GetModifierInput(bool once = false); // returns if shift/ctrl/alt is pressed

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
