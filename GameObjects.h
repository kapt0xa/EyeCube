#pragma once
#include "linal.h"
#include "KeyboardInput.h"

#include <array>
#include <unordered_map>
#include <string>

namespace cube_game
{
	constexpr int field_size = 2;
	constexpr int side_size = field_size * 2 - 1;
	using VecToVecMap = std::unordered_map<V3i, V3i>;
	using VecToMatMap = std::unordered_map<V3i, M3i>;

	const M3i rot_up =		M3i(V3i::I(), -V3i::K(), V3i::J());
	const M3i rot_down =	M3i(V3i::I(), V3i::K(), -V3i::J());
	const M3i rot_left =	M3i(V3i::K(), V3i::J(), -V3i::I());
	const M3i rot_right =	M3i(-V3i::K(), V3i::J(), V3i::I());
	const M3i yaw_left =	M3i(V3i::J(), -V3i::I(), V3i::K());
	const M3i yaw_right =	M3i(-V3i::J(), V3i::I(), V3i::K());

	const V3i vacant_chip = V3i::K() * field_size;

	const std::array<M3i, 6> cube_perspectives =
	{
		M3i::Identity(),	// [0]
		rot_down,			// [1]
		rot_up,				// [2]
		rot_left,			// [3]
		rot_right,			// [4]
		rot_up * rot_up		// [5]
	};

	const VecToMatMap rotations =
	{
		{ V3i::I(), rot_up },
		{ -V3i::I(), rot_down },
		{ -V3i::J(), rot_left },
		{ V3i::J(), rot_right },
		{ V3i::K(), yaw_left },
		{ -V3i::K(), yaw_right }
	};

	class PositionProcesser
	{
	public:
		static V3i GetMovedPosition(const V3i pos, const V2i& direction, const M3i& perspective);

		static VecToVecMap BuildMap();

		static V3i Shuffle(VecToVecMap& map); // returns position of vacant chip

		static V3i GetRelativeDeviation(const V3i& current_pos, const V3i& target_pos, const M3i& perspective);

		static std::array<std::array<V3i, field_size * 2 - 1>, field_size * 2 - 1>
			BuildSideSample();
	};

	class MapHolder
	{
	public:
		M3i perspective;
		V3i vacant_pos;
		MapHolder();
		std::string RenderField() const;
		void MoveChip(const V3i& direction);

	private:	
		std::array<std::array<V3i, field_size * 2 - 1>, field_size * 2 - 1>
			side_sample;
		VecToVecMap map;
	};
}