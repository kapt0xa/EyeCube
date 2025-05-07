#include <Windows.h>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

int main()
{
	do
	{
		auto respond = GetAsyncKeyState(VK_MBUTTON);
		cout << ((respond & 0x8000) ? "*" : " ") << "\t" << flush << ((respond & 1) ? "*" : " ") << endl;
		this_thread::sleep_for(chrono::milliseconds(100));

	} while (!GetAsyncKeyState(VK_ESCAPE));
	while (GetAsyncKeyState(VK_ESCAPE));
}
