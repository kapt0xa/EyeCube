#include <Windows.h>
#include "KeyboardInput.h"

namespace cube_game
	{
	constexpr int KeyPressedFlag = 0x8000;

	const V2f KeyboardInput::up = {0, 1};
	const V2f KeyboardInput::down = {0, -1};
	const V2f KeyboardInput::left = { -1, 0 };
	const V2f KeyboardInput::right = { 1, 0 };
	const V2f KeyboardInput::zero = { 0, 0 };
	const float KeyboardInput::yes = 1;
	const float KeyboardInput::no = -1;

	constexpr int key_pressed = 0x8000;
	constexpr int key_pressed_once = 0x8000;
	V2f KeyboardInput::GetMainInput(bool once)
	{
		auto bitmask = once? key_pressed : key_pressed_once;
		V2f result = zero;
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
		return result;
	}

	float KeyboardInput::GetLogicInpit(bool once)
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