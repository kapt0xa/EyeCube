#pragma once
#include "linal.h"
#include "KeyboardInput.h"

#include <array>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

namespace cube_game
{
	class Renderer // class for constructing a multi-line string that can be printed as immage
	{
	public:
		Renderer(const V2i& canvas_size);

		// each line should have '\n' at the end and this char is not included i the canvas_size variable
		const V2i canvas_size; // .X() and .Y() can be used to access cords. V2I(a, b) -thats how u can build vector

		using Sprite = std::vector<std::string>; // used for storing partical immage

		Sprite GetSprite(V2i begin, V2i size) const;
		void PutSprite(const Sprite& sprite, V2i begin, V2i repeat = V2i::Ones(), V2i step = V2i::Ones());
		void PutSprite(const std::string& sprite, V2i begin, V2i repeat = V2i::Ones(), V2i step = V2i::Ones());
		void PutSprite(const char sprite, V2i begin, V2i repeat = V2i::Ones(), V2i step = V2i::Ones());

		void Clear(char filler = ' ');

		const std::string& GetCanvas() const; // returns the whole canvas as a string

		const char& operator[](V2i pos) const; // operator [] is used for DRY, to reference address by coordinate in one style
		char& operator[](V2i pos);

	private:
		std::string canvas;
	};

	// game is like 15-puzzle, it is about moving chips. but the field is not flat-shaped. it is cube-surface shape.
	// in final version all that data might be not required, but for now i want more ways to see what's happenning for debug purposes
	// chips can not be numbered in perfectionist way, and would be drawn as arrows to their home, or as distance to home
	// as integers are technically easier due calculations without error, the field is simmetrical and without halfs.
	// so, the field is represented by cubes 1x1x1, 3x3x3, 5x5x5 ...
	// original 15-puzzle has field with size of 16.
	// 1x1x1 cube has 6 positions. it is too boring
	// 3x3x3 cube has 9*6 = 54 positions, it seems enought, but a bit much
	// 2x2x2 would be perfect (24 positions), but it needs some modifications about skipping even positions (-1, 0, +1) -> (-1, +1)
	// first would be nice to make 
	// i suppose that unlike classical 15-puzzle, that can become unsolvable because of incorrect shuffle, this puzzle can be solved from any shufle 
	struct ChipData
	{
		const V3i home_pos; // home position of chip
		const V3i home_dir; // this variable represents one of 6 cube's sides. or direction of that side
		const int field_size; // that doesn't represent any significant property of field. it is just number, which the field is based on. each position of field has that coordinate in direction of side. only one of 3 coordinates is +- that ammount, other cords are less by absolute.
		char symbol; // for debug purposes
		std::string name; // for debug purposes
		const int id; // for debug purposes, same as home_pos id

		// if name is empty, generate name from id, symbol and position
		void AutoName();
		// returns ><^vX0 depending on deviation from home_pos and perspective.
		// 0 - chip is in position (actual_pos = home_pos), X - chip is on opposite side
		// as cube could be rotated different ways, arrow depends on perspective.
		// rotations are presented with 3x3 matrixes instead of quaternions, because integer matrixes dont accumulate error or absolute value.
		char GetArrow(V3i actual_pos, M3i perspective) const;
		// arternnative way of representating chip.
		// difficult and good implementation - minimal ammount of steps to home
		// easier and good enought implementation - coordinate difference (abs(delta x)+abs(delta y)+abs(delta z))
		int GetDistanceFromHomePos(const V3i& actual_pos) const; // returns distance from home_pos
	};

	class Field
	{
	public:
		template<typename T>
		using Map = std::unordered_map<V3i, T, HV3i>;
	private:
		struct PosData
		{
			std::unique_ptr<ChipData> chip; // unique ptr is used for fast swapping because it is data about chip that is moved a lot
			const V3i direction; // can be calculated by finding the direction of abs(cord) -max. or others ways (maybe pos/size in case of integer division). stored not to calculate it again and again
			const int id; // represents index in std::vector
		};
		Map<PosData> map; // the field with cips
		std::vector<const V3i> positions; // represents connection id-position. it's copy may be used for copying
		Map<std::pair<int, int>> sides; // direction -> id_begin, size, used to list positions that are on side
		int common_size; // = 2. might be changed for increasing field size
		bool is_even; // might be not implemented. affects skipping odd positions and increasing step into 2
		V3i vacant_pos; // used to track the vacant pos.
	public:
		const ChipData& operator[](const V3i& pos) const; // interface just in case if needed for debug or something
		const V3i& GetVacantPos() const;
		const V3i& GetDirection(const V3i pos) const;
		const V3i& GetVacantDirection() const; // interface for renderer
		bool MoveChip(const V3i& direction); // returns true if chip moved, interface for controller. hint: if it returns false, perspective or something else was used incorrectly
		std::pair<const V3i*, int> GetPossOnSide(const V3i& direction) const; // returns pointer to array of chips and size
		Field& BuildNew(int size, bool is_even);
		Field& Shuffle(int seed);
		Field& Shuffle(); // uses time as seed. remember about DRY
	private:
		int CenterOffset() const; // = common_size
		int SideRadius() const; // = common_size - 1
		int SideSize() const; // = common_size * 2 - 1
		int ChipFillStep() const; // is_even ? 2:1
		int ChipStripLen() const; // is_even ? ((common_size / 2) * 2) : common_size * 2 - 1

		static std::vector<const V3i> BuildSide(int common_size, bool is_even = false); // z = common_size, x is from -common_size+1 to common_size-1, skips odd positions if is_even
		static const std::array<M3i, 6>& EnumerateSides(); // used to rotate 1 side to make 6
		static std::vector<const V3i> BuildField(int common_size, bool is_even = false); // is build from 6 sides

		V3i NormaliseOutOfSide(const V3i& out_of_bounds_pos, const V3i& last_move) const; // used to link chips on edges to other side.
		V3i VacantHomePos() const; // returns home position of vacant pos. it is used when field is build or shuffled
	};

	class FieldRenderer
	{
	private:
		Field& field;
		V3i last_direction; // used to determine if perspective need to be updated
		M3i perspective; // persprective is above vacant position, vacant pos direction should be up
		Renderer renderer;
		int separator_size = 10; // ammount of '\n' before each render
		bool use_arrows = true;
	public:
		FieldRenderer(Field& field); // renders 5 of 6 sides (dont render opposite side)
		const M3i& GetPerspective() const; // interface for controller
		Field& GetField() const; // interface for controller
		void FixPerspective(V2i last_move); // interface for controller, changes perspective if vacant pos changes side.
		void YawPerspective(int direction);
		void Render() const; // prints into cout.
		void SwitchRenderType(); // switches between arrows and numbers
	};

	class Controller
	{
	private:
		FieldRenderer& field_renderer;
		Field& field;
		KeyboardInput input;
	public:
		Controller(FieldRenderer& field_renderer);
		bool ProcessInput(); // returns false if game is closed. occupys thread untill KeyboardInput returns something, applys change after kay is released. ignores abbugous input
		bool ProcessInputAndRender();
	private:
		void MoveChip(const V2i& direction); // depends on perspective. changes V2i->V3i.
		void SwitchRenderType();
	};

};