#include "common.hpp"
#include "math.hpp"
#include "memory.hpp"

class LootItem
{
public:
	static bool query_item_id(std::string file_input, std::string id, std::string& short_name, uint32_t& price)
	{
		size_t offset = file_input.find(id);

		if (offset == std::string::npos)
			return false;

		offset = file_input.find(": \"", offset) + 3;
		size_t temp = file_input.find("\",", offset);
		short_name = file_input.substr(offset, temp - offset);

		offset = file_input.find("Price\": ", temp) + 8;
		temp = file_input.find("},", offset) - 13;
		price = std::stoi(file_input.substr(offset, temp - offset));
	}

	std::string get_name()
	{
		uintptr_t name_pointer = memory::read<uintptr_t>((uintptr_t)this + 0x50);
		const int name_size = memory::read<int>(name_pointer + 0x10) - 10;
		return memory::get_unicode_str(name_pointer, name_size);
	}

	std::string get_item_id()
	{
		uintptr_t name_pointer = memory::read<uintptr_t>((uintptr_t)this + 0x60);
		const int name_size = memory::read<int>(name_pointer + 0x10);
		return memory::get_unicode_str(name_pointer, name_size);
	}

	Vector3 get_position()
	{
		auto transform = memory::read_chain((uintptr_t)this, { 0x10, 0x30, 0x30, 0x8, 0x28, 0x10 });
		return Bone::get_transform_position(transform);
	}
};

class WorldLoot
{
public:
	uint32_t price;
	Vector2 location;
	std::string short_name;
	std::string id;
};

class Loot_List
{
public:
	std::vector<uintptr_t> loot_pointer_list{};
	std::vector<WorldLoot> world_loot_list;
} loot_list;