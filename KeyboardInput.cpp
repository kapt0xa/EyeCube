#include <Windows.h>
#include "KeyboardInput.h"

namespace cube_game
	{
	const V3i KeyboardInput::up = V3i::J();
	const V3i KeyboardInput::down = -V3i::J();
	const V3i KeyboardInput::left = -V3i::I();
	const V3i KeyboardInput::right = V3i::I();
	const V3i KeyboardInput::zero = { 0, 0, 0 };
	const V3i KeyboardInput::yaw_left = { 0, 0, 1 };
	const V3i KeyboardInput::jaw_right = { 0, 0, -1 };
	const int KeyboardInput::yes = 1;
	const int KeyboardInput::no = -1;

	constexpr int key_pressed = 0x8000;
	constexpr int key_pressed_once = 0x8000;
	V3i KeyboardInput::GetMainInput(bool once)
	{
		auto bitmask = once? key_pressed : key_pressed_once;
		V3i result = zero;
		if (GetAsyncKeyState('W') & bitmask
			|| GetAsyncKeyState(VK_UP) & bitmask)
		{
			result += up;
		}
		if (GetAsyncKeyState('S') & bitmask
			|| GetAsyncKeyState(VK_DOWN) & bitmask)
		{
			result += down;
		}
		if (GetAsyncKeyState('A') & bitmask
			|| GetAsyncKeyState(VK_LEFT) & bitmask)
		{
			result += left;
		}
		if (GetAsyncKeyState('D') & bitmask
			|| GetAsyncKeyState(VK_RIGHT) & bitmask)
		{
			result += right;
		}
		if (GetAsyncKeyState('Q') & bitmask)
		{
			result += right;
		}
		if (GetAsyncKeyState('E') & bitmask)
		{
			result += right;
		}
		return result;
	}

	int KeyboardInput::GetLogicInpit(bool once)
	{
		auto bitmask = once ? key_pressed : key_pressed_once;
		if (GetAsyncKeyState(VK_RETURN) & bitmask)
		{
			return yes;
		}
		if (GetAsyncKeyState(VK_ESCAPE) & bitmask)
		{
			return no;
		}
		return 0;
	}

	bool KeyboardInput::GetModifierInput(bool once)
	{
		auto bitmask = once ? key_pressed : key_pressed_once;
		if (GetAsyncKeyState(VK_SHIFT) & bitmask)
		{
			return true;
		}
		if (GetAsyncKeyState(VK_CONTROL) & bitmask)
		{
			return true;
		}
		if (GetAsyncKeyState(VK_MENU) & bitmask)
		{
			return true;
		}
		return false;
	}
}