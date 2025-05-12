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
	class Renderer
	{
	public:
		Renderer(const V2i& canvas_size);

		const V2i canvas_size;

		using Sprite = std::vector<std::string>;

		Sprite GetSprite(V2i begin, V2i size) const;
		void PutSprite(const Sprite& sprite, V2i begin, V2i repeat = V2i::Ones(), V2i step = V2i::Ones());
		void PutSprite(const std::string& sprite, V2i begin, V2i repeat = V2i::Ones(), V2i step = V2i::Ones());
		void PutSprite(const char sprite, V2i begin, V2i repeat = V2i::Ones(), V2i step = V2i::Ones());

		void Clear(char filler = ' ');

		const std::string& GetCanvas() const; // returns the whole canvas as a string

		const char& operator[](V2i pos) const;
		char& operator[](V2i pos);

	private:
		std::string canvas;
	};

	struct ChipData
	{
		const V3i home_pos;
		const V3i home_dir;
		int field_size;
		char symbol;
		std::string name;
		int id;

		// if name is empty, generate name from id, symbol and position
		void AutoName();
		char GetArrow(V3i actual_pos, M3i perspective) const; // returns ><^vX0 depending on deviation from home_pos and perspective
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
			std::unique_ptr<ChipData> chip;
			const V3i direction;
			int id;
		};
		Map<PosData> map;
		std::vector<V3i> positions;
		Map<std::pair<int, int>> sides; // direction -> id_begin, size
		int common_size;
		bool is_even;
		V3i vacant_pos;
	public:
		const ChipData& operator[](const V3i& pos) const;
		V3i GetVacantPos() const;
		const V3i& GetDirection(const V3i pos) const;
		const V3i& GetVacantDirection() const;
		bool MoveChip(const V3i& direction); // returns true if chip moved
		std::pair<const V3i*, int> GetChipsOnSide(const V3i& direction) const; // returns pointer to array of chips and size
		Field& BuildNew(int size, bool is_even);
	private:
		int CenterOffset() const;
		int SideSize() const;
		int ChipFillStep() const;
		int ChipStripLen() const;
	};

	class FieldRenderer
	{
	private:
		Field& field;
		V3i last_direction;
		M3i perspective;
		Renderer renderer;
		int separator_size = 10;
		bool use_arrows = true;
	public:
		FieldRenderer(Field& field);
		const M3i& GetPerspective() const;
		Field& GetField() const;
		void Render() const;
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
		void ProcessInput();
	private:
		void MoveChip(const V2i& direction);
		void FixPerspective();
		void YawPerspective(int direction);
		void SwitchRenderType();
	};

};