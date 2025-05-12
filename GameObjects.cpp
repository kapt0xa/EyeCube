#include "GameObjects.h"

#include <utility>

namespace cube_game
{
	Renderer::Renderer(const V2i& canvas_size)
		: canvas_size(canvas_size)
	{
		canvas.resize((canvas_size.X() +1) * canvas_size.Y(), ' ');
		Clear();
	}

	using Sprite = Renderer::Sprite;
	Sprite Renderer::GetSprite(V2i begin, V2i size) const
	{
		Sprite sprite;
		sprite.reserve(size.Y());
		for (int j = begin.Y(); j < size.Y(); ++j)
		{
			const char* str_begin = &((*this)[V2i(begin.X(), j)]);
			const int str_size = size.X();
			sprite.push_back(std::string(str_begin, str_size));
		}
		return sprite;
	}

	void Renderer::PutSprite(const Sprite& sprite, V2i begin, V2i repeat, V2i step)
	{
		for (int y_repeat = 0; y_repeat < repeat.Y(); ++y_repeat)
			for (int x_repeat = 0; x_repeat < repeat.X(); ++x_repeat)
			{
				auto sub_begin = begin + V2i(x_repeat * step.X(), y_repeat * step.Y());
				for (int j = 0; j < sprite.size(); ++j)
					for (int i = 0; i < sprite[j].size(); ++i)
					{
						(*this)[sub_begin + V2i(i, j)] = sprite[j][i];
					}
			}
	}
	void Renderer::PutSprite(const std::string& sprite, V2i begin, V2i repeat, V2i step)
	{
		Sprite sprite_vec = { sprite };
		PutSprite(sprite_vec, begin, repeat, step);
	}
	void Renderer::PutSprite(const char sprite, V2i begin, V2i repeat, V2i step)
	{
		Sprite sprite_vec = { {sprite} };
		PutSprite(sprite_vec, begin, repeat, step);
	}
	void Renderer::Clear(char filler)
	{
		std::fill(canvas.begin(), canvas.end(), filler);
		for (int i = 0; i < canvas_size.Y(); ++i)
		{
			(*this)[V2i(canvas_size.X(), i)] = '\n';
		}
	}
	const std::string& Renderer::GetCanvas() const
	{
		return canvas;
	}
	const char& Renderer::operator[](V2i pos) const
	{
		return canvas[pos.Y() * (canvas_size.X() + 1) + pos.X()];
	}
	char& Renderer::operator[](V2i pos)
	{
		return const_cast<char&>(const_cast<const Renderer&>(*this)[pos]);
	}
	void ChipData::AutoName()
	{
		if (name.empty())
		{
			name = 
				std::to_string(id) + ' ' +
				symbol + ' ' + 
				std::to_string(home_pos.X()) + "," +
				std::to_string(home_pos.Y()) + "," +
				std::to_string(home_pos.Z());
		}
	}
	char ChipData::GetArrow(V3i actual_pos, M3i perspective) const
	{
		auto deviation = perspective * (home_pos - actual_pos);
		if (deviation.Z() == 2 * field_size)
			return 'X';
		if (deviation == V3i::Zero())
			return '0';
		bool horizontal = (deviation.X() > deviation.Y());
		if (deviation.X() == deviation.Y())
		{
			horizontal = 1 == ((deviation.X() ^ deviation.Z()) & 1);
		}
		return horizontal ?
			(deviation.X() < 0 ? '<' : '>') :
			(deviation.Y() < 0 ? 'v' : '^');
	}
};
