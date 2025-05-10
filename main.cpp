#include "GameObjects.h"

#include <iostream>

using namespace std;

void Play();

int main()
{
	Play();
}

void Play()
{
	using namespace cube_game;
	KeyboardInput input;
	MapHolder map;
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
		<< map.RenderField() << flush;
	while (true)
	{
		if (input.GetLogicInpit() == -1)
		{
			while (input.GetLogicInpit() != 0);
			break;
		}
		auto action = input.GetMainInput();
		if (action != V3i::Zero())
		{
			while (input.GetMainInput() != V3i::Zero());
			map.MoveChip(action);
			std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
				<< map.RenderField() << flush;
		}
	}
}
