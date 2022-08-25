#include "common.hpp"
#include "math.hpp"
#include "memory.hpp"

struct Extract_Info
{
	std::string name;
	Vector2 screen_pos;
};

class ExfiltrationPoint
{
public:
	Vector3 get_position()
	{
		auto transform = memory::read_chain((uintptr_t)this, { 0x10, 0x30, 0x30, 0x8, 0x28, 0x10 });
		return Bone::get_transform_position(transform);
	}

	std::string get_name()
	{
		uintptr_t settings = memory::read<uintptr_t>((uintptr_t)this + 0x58);
		
		uintptr_t name_pointer = memory::read<uintptr_t>(settings + 0x10);
		const int name_size = memory::read<int>(name_pointer + 0x10);

		return memory::get_unicode_str(name_pointer, name_size);
	}
};

class Exfil_List
{
public:
	std::vector<uintptr_t> exfil_pointer_list;
	std::vector<Extract_Info> extract_info_list;
} exfil_list;