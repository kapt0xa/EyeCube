#include "GameObjects.h"

#include <vector>
#include <random>
#include <chrono>
#include <cassert>
#include <cmath>
#include <iostream>

namespace cube_game
{
	V3i PositionProcesser::GetMovedPosition(const V3i pos, const V2i& direction, const M3i& perspective)
	{
		auto oppos_persp = perspective.Transponeered();
		assert((oppos_persp * pos).Z() == field_size);
		assert(std::abs((oppos_persp * pos).X()) < field_size);
		assert(std::abs((oppos_persp * pos).Y()) < field_size);

		assert(perspective.X().Magnitude2() == 1);
		assert(perspective.Y().Magnitude2() == 1);
		assert(perspective.Z().Magnitude2() == 1);
		assert(perspective.Det() == 1);

		assert(direction.Magnitude2() == 1);
		assert((direction.X() == 0) ^ (direction.Y() == 0));
		
		auto shift = perspective * direction.Resized<3>();
		assert(shift.Magnitude2() == 1);
		assert((shift.X() != 0) ^ (shift.Y() != 0) ^ (shift.Z() != 0)); // ammount of not null is odd - 1 or 3 
		assert((shift.X() == 0) || (shift.Y() == 0) || (shift.Z() == 0)); // at least one cord is 0

		int axis = 0;
		for (;; ++axis) // finds whitch axis is the direction of shift;
		{
			assert(axis < 3);
			if (shift[axis] != 0)
			{
				break;
			}
		}

		auto result = pos + shift;
		if (std::abs(result[axis]) == field_size)
		{
			int secondary_axis = 0;
			for (;; ++secondary_axis) // finds axis of sube side
			{
				assert(secondary_axis < 3);
				if (std::abs(pos[secondary_axis]) == field_size)
				{
					break;
				}
			}

			result[secondary_axis] += (result[secondary_axis] < 0) ? 1 : -1;
		}

		return result;
	}

	VecToVecMap PositionProcesser::BuildMap()
	{
		VecToVecMap map;
		auto side_sample = BuildSideSample();
		for (const auto& persp : cube_perspectives)
		{
			//std::cerr << "----------------------new perspective----------------------\n";
			for (const auto& line : side_sample)
				for (const auto& sample_pos : line)
				{
					auto pos = persp * sample_pos;
					auto [_, unique_place] = map.try_emplace(pos, pos);
					//std::cerr << pos.X() << '\t' << pos.Y() << '\t' << pos.Z() << '\n';
					assert(unique_place); // position already exists
				}
		}
		assert(map.find(vacant_chip) != map.end());
		return map;
	}

	V3i PositionProcesser::Shuffle(std::unordered_map<V3i, V3i>& map)
	{
		std::vector<V3i*> positions;
		positions.reserve(map.size());
		for (auto& [pos, chip] : map)
		{
			positions.emplace_back(&chip);
		}

        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 gen(seed);
        std::shuffle(positions.begin(), positions.end(), gen);

		V3i* vacant_ptr = nullptr;
		for (auto& [_, chip] : map)
		{
			chip = *positions.back();
			positions.pop_back();
			if (chip == vacant_chip)
			{
				vacant_ptr = &chip;
			}
		}

		int fail_count = 0;
		for (auto& [pos, chip] : map)
		{
			if (map.find(chip) == map.end())
			{
				std::cerr << "chip not found in map\n";
				std::cerr << "chip: " << chip.X() << '\t' << chip.Y() << '\t' << chip.Z() << '\n';
				++fail_count;
			}
			if (map.find(pos) == map.end())
			{
				std::cerr << "position not found in map\n";
				std::cerr << "position: " << pos.X() << '\t' << pos.Y() << '\t' << pos.Z() << '\n';
				++fail_count;
			}
		}
		assert(fail_count == 0); // map is not consistent

		assert(vacant_ptr != nullptr); // vacant chip not found

		return *vacant_ptr;

		assert(false); // vacant chip not found
	}

	V3i PositionProcesser::GetRelativeDeviation(const V3i& current_pos, const V3i& target_pos, const M3i& perspective)
	{
		return perspective.Transponeered()*(target_pos - current_pos);
	}

	std::array<std::array<V3i, field_size * 2 - 1>, field_size * 2 - 1> PositionProcesser::BuildSideSample()
	{
		std::array<std::array<V3i, field_size * 2 - 1>, field_size * 2 - 1>
			side_sample;
		for (int i = 0, x = 1 - field_size; x < field_size; ++i, ++x)
			for (int j = 0, y = 1 - field_size; y < field_size; ++j, ++y)
			{
				side_sample[i][j] = V3i(x, y, field_size);
			}
		return side_sample;
	}

	MapHolder::MapHolder()
	{
		map = PositionProcesser::BuildMap();
		vacant_pos = PositionProcesser::Shuffle(map); //problem
		perspective = M3i::Identity();

		side_sample = PositionProcesser::BuildSideSample();

		auto direction = vacant_pos / field_size;
		assert(direction.Magnitude2() == 1);
		if (direction.Z() != 0)
		{
			if (direction.Z() == -1)
			{
				perspective = rot_up * rot_up;
			}
			assert(perspective.Det() == 1);
			assert(perspective.X().Magnitude2() == 1);
			assert(perspective.Y().Magnitude2() == 1);
			assert(perspective.Z().Magnitude2() == 1);
			return;
		}
		perspective.Z() = direction;
		if (perspective.X().Dot(direction) != 0)
		{
			perspective.Y() = perspective.Z().Cross(perspective.X());
		}
		else
		{
			perspective.X() = perspective.Y().Cross(perspective.Z());
		}
		assert(perspective.Det() == 1);
		assert(perspective.X().Magnitude2() == 1);
		assert(perspective.Y().Magnitude2() == 1);
		assert(perspective.Z().Magnitude2() == 1);
	}

	std::string MapHolder::RenderField() const
	{
		using Str = std::string;
		Str field =
			//	 0000000000111111111122222222
			//	 0123456789012345678901234567
				"         [0][u][p]         \n"	// 0 * 28
				"         [1][u][p]         \n" // 1 * 28
				"         [2][u][p]         \n" // 2 * 28
				"[0][l][f][0][i][d][0][r][g]\n" // 3 * 28
				"[1][l][f][1][i][d][1][r][g]\n" // 4 * 28
				"[2][l][f][2][i][d][2][r][g]\n" // 5 * 28
				"         [0][d][n]         \n" // 6 * 28
				"         [1][d][n]         \n" // 7 * 28
				"         [2][d][n]         \n";// 8 * 28
		constexpr int chip_size = 3;
		constexpr int chip_offset = 1;
		constexpr int side_offset_x = side_size * chip_size;
		constexpr int line_length = side_offset_x * 3 + 1; // =28
		constexpr int side_offset_y = side_size * line_length;
		constexpr int field_render_size = side_offset_y * 3;

		assert(field.size() == field_render_size);

		M3i current_persp = perspective;
		int current_pos = 1 * side_offset_y + 1 * side_offset_x + chip_offset;

		auto perspective_filler = [&]()
			{
				for (int i = 0; i != side_size; ++i)
					for (int j = 0; j != side_size; ++j)
					{
						char& pixel = field[current_pos + i * chip_size + j * line_length];
						auto pos = current_persp * side_sample[i][j];
						const auto& chip_home = map.at(pos);
						if (chip_home == vacant_chip)
						{
							pixel = ' ';
						}
						else
						{
							auto deviation = PositionProcesser::GetRelativeDeviation(pos, chip_home, perspective);
							if (deviation.Z() == -2 * field_size)
							{
								pixel = 'X';
							}
							else if (deviation == V3i::Zero())
							{
								pixel = '*';
							}
							else
							{
								int d_h = std::abs(deviation.X());
								int d_v = std::abs(deviation.Y());
								bool horisontal = d_h > d_v;
								if (d_h == d_v)
								{
									horisontal = ((d_h ^ pos.Hash()) % 2) == 0;
								}
								if (horisontal)
								{
									pixel = (deviation.X() < 0) ? '<' : '>';
								}
								else
								{
									pixel = (deviation.Y() < 0) ? '^' : 'v';
								}
							}
						}
					}
			};
		perspective_filler();

		current_persp = perspective * rot_up;
		current_pos = 0 * side_offset_y + 1 * side_offset_x + chip_offset;
		perspective_filler();

		current_persp = perspective * rot_down;
		current_pos = 2 * side_offset_y + 1 * side_offset_x + chip_offset;
		perspective_filler();

		current_persp = perspective * rot_left;
		current_pos = 1 * side_offset_y + 0 * side_offset_x + chip_offset;
		perspective_filler();

		current_persp = perspective * rot_right;
		current_pos = 1 * side_offset_y + 2 * side_offset_x + chip_offset;
		perspective_filler();

		return field;
	}

	void MapHolder::MoveChip(const V3i& direction)
	{
		assert(direction.Magnitude2() == 1);

		if (direction.Z() != 0)
		{
			perspective = perspective * rotations.at(direction);
		}
		else
		{
			const auto swapped_chip = PositionProcesser::GetMovedPosition(vacant_pos, direction.Prefix<2>(), perspective);
			auto old_direction = vacant_pos / field_size;
			auto new_direction = swapped_chip / field_size;
			if (old_direction != new_direction)
			{
				assert(old_direction.Magnitude2() == 1);
				assert(new_direction.Magnitude2() == 1);
				
				perspective = perspective * rotations.at(new_direction);
			}
			std::swap(map.at(vacant_pos), map.at(swapped_chip));
			vacant_pos = swapped_chip;
		}
	}
}
