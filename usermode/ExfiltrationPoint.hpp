#include "common.hpp"
#include "math.hpp"
#include "memory.hpp"

class ExfiltrationPoint
{
public:
	Vector3 get_position_test()
	{
		auto a1 = memory::read_chain((uintptr_t)this, { 0x48, 0x30, 0x8, 0x28, 0x10 });
		return Bone::get_transform_position(a1);
	}
	std::string get_name()
	{
		uintptr_t settings = memory::read<uintptr_t>((uintptr_t)this + 0x58);
		
		uintptr_t name_pointer = memory::read<uintptr_t>(settings + 0x10);
		const int name_size = memory::read<int>(name_pointer + 0x10);

		return memory::get_unicode_str(name_pointer, name_size);
	}

};