#include "GameObjects.h"

#include <random>
#include <ctime>
#include <random>

//
// GPT:
// I've written the core of GameObjects.cpp based on your provided headers and style. Some functions like FixPerspective, YawPerspective, and actual Render() logic are marked as TODO since they require more rendering/perspective logic.
// Would you like help finishing those rendering and rotation parts next ?
//

namespace cube_game
{
	Renderer::Renderer(const V2i& canvas_size)
		: canvas_size(canvas_size)
	{
		canvas.resize((canvas_size.X() + 1) * canvas_size.Y(), ' ');
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

	int ChipData::GetDistanceFromHomePos(const V3i& actual_pos) const
	{
		V3i delta = home_pos - actual_pos;
		return std::abs(delta.X()) + std::abs(delta.Y()) + std::abs(delta.Z());
	} // written by GPT, checked by me

	const ChipData& Field::operator[](const V3i& pos) const
	{
		return *map.at(pos).chip;
	} // written by GPT, checked by me

	const V3i& Field::GetVacantPos() const
	{
		return vacant_pos;
	} // written by GPT, checked by me

	const V3i& Field::GetDirection(const V3i pos) const
	{
		return map.at(pos).direction;
	} // written by GPT, checked by me

	const V3i& Field::GetVacantDirection() const
	{
		return GetDirection(vacant_pos);
	} // written by GPT, checked by me

	bool Field::MoveChip(const V3i& direction)
	{
		V3i new_pos = vacant_pos + direction;
		auto it = map.find(new_pos);
		if (it == map.end()) return false;
		map[vacant_pos].chip.swap(it->second.chip);
		vacant_pos = new_pos;
		return true;
		incorrect, side change not implemented;
	}

	std::pair<const V3i*, int> Field::GetPossOnSide(const V3i& direction) const
	{
		auto data = sides.at(direction);
		return { positions.data() + data.first, data.second };
	} // written by GPT, checked by me

	Field& Field::BuildNew(int size, bool is_even)
	{
		this->common_size = size;
		this->is_even = is_even;
		positions = BuildField(size, is_even);
		map.clear();
		auto& persps = EnumerateSides();
		sides.clear();
		int id_counter = 0;

		auto side_sample = BuildSide(size, is_even);
		for (int i = 0; i < persps.size(); ++i)
		{
			V3i dir = persps[i] * V3i::K();
			for (auto& p : side_sample)
			{
				V3i rotated = persps[i] * p;
				auto chip = std::make_unique<ChipData>(ChipData{ rotated, persps[i].Z() * V3i::K(), size, '*', {}, id_counter});
				chip->AutoName();
				auto [_, is_emplaced] = map.emplace(rotated, PosData{ std::move(chip), persps[i] * V3i::K(), id_counter});
				++id_counter;
				positions.push_back(rotated);
			}
            sides[dir] = { static_cast<int>(positions.size() - side_sample.size()), static_cast<int>(side_sample.size()) };
		}

		vacant_pos = VacantHomePos();
		return *this;
	} // written by GPT, checked by me

	Field& Field::Shuffle(int seed)
	{
		std::vector<std::unique_ptr<ChipData>> shuffled;
		shuffled.reserve(positions.size());
		for (int i = 0; i < positions.size(); ++i)
		{
			shuffled.emplace_back(map[positions[i]].chip.release());
		}
		std::mt19937 g(seed);
		auto vacant_home_pos = VacantHomePos();
		for (auto& [pos, data] : map)
		{
			if (shuffled.back()->home_pos == vacant_home_pos)
			{
				vacant_pos = pos;
			}
			data.chip.reset(shuffled.back().release());
			shuffled.pop_back();
		}
		std::shuffle(shuffled.begin(), shuffled.end(), g);

		return *this;
	} // written by GPT, checked by me

	Field& Field::Shuffle()
	{
		return Shuffle(static_cast<int>(std::time(nullptr)));
	} // written by GPT, checked by me

	int Field::CenterOffset() const { return common_size; }
	int Field::SideRadius() const { return common_size - 1; }
	int Field::SideSize() const { return common_size * 2 - 1; }
	int Field::ChipFillStep() const { return is_even ? 2 : 1; }
	int Field::ChipStripLen() const { return is_even ? ((common_size / 2) * 2) : (common_size * 2 - 1); }
	// written by GPT, checked by me

	FieldRenderer::FieldRenderer(Field& field)
		: field(field)
		, renderer({ 30, 9 }) // adjust canvas size as needed
	{
		last_direction = field.GetVacantDirection();
		perspective = M3i::Identity();
		FixPerspective(V2i::Zero());
	}// written by GPT, checked by me

	const M3i& FieldRenderer::GetPerspective() const { return perspective; }
	Field& FieldRenderer::GetField() const { return field; }

	void FieldRenderer::FixPerspective(V2i last_move)
	{
		V3i new_dir = field.GetVacantDirection();
		if (new_dir == last_direction) return;
		perspective.Z() = new_dir;
		if (last_move.X() == 0)
		{
			perspective.Y() = perspective.Z().Cross(perspective.X());
		}
		else
		{
			perspective.X() = perspective.Y().Cross(perspective.Z());
		}
		assert(perspective.Det() == 1);
	}// written by GPT, checked by me

	void FieldRenderer::YawPerspective(int direction)
	{
		if (direction < 0)
		{
			auto buf = perspective.X();
			perspective.X() = perspective.Y();
			perspective.Y() = -buf;
		}
		else
		{
			auto buf = perspective.X();
			perspective.X() = -perspective.Y();
			perspective.Y() = buf;
		}
		assert(perspective.Det() == 1);
	}

	void FieldRenderer::Render() const
	{
		work here;
		// TODO: render 5 sides using field data and perspective
	}

	void FieldRenderer::SwitchRenderType()
	{
		use_arrows = !use_arrows;
	}

	Controller::Controller(FieldRenderer& field_renderer)
		: field_renderer(field_renderer), field(field_renderer.GetField()) {
	}

	bool Controller::ProcessInput()
	{
		V3i input_vec = input.GetMainInput(true);
		if (input_vec == V3i::Zero()) return true;
		return field.MoveChip(input_vec);
	}

	bool Controller::ProcessInputAndRender()
	{
		bool running = ProcessInput();
		field_renderer.Render();
		return running;
	}

	void Controller::MoveChip(const V2i& direction)
	{
		// TODO: convert V2i to V3i depending on perspective and move chip
	}

	void Controller::SwitchRenderType()
	{
		field_renderer.SwitchRenderType();
	}

}
