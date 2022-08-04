#pragma once
#include "common.hpp"
#include "memory.hpp"

class World
{
private:
	struct list_info
	{
		int size;
		uintptr_t base;
	};
public:
	std::vector<uintptr_t> get_player_list()
	{
		std::vector<uintptr_t> return_list;

		uintptr_t list_ptr = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x88);
		if (!list_ptr)
			return return_list;

		list_info info{};

		info.size = memory::read<int>(list_ptr + 0x18);
		info.base = memory::read<uintptr_t>(list_ptr + 0x10);
		if (!info.base || info.size > 200)
			return return_list;

		std::unique_ptr<uintptr_t> entities_buffer(new uintptr_t[info.size * sizeof(uintptr_t)]);

		/* read into the buffer */
		memory::copy_memory(info.base + 0x20, (uintptr_t)entities_buffer.get(), info.size * sizeof(uintptr_t));
		return_list = { entities_buffer.get(), entities_buffer.get() + (uintptr_t)info.size };
		return return_list;
	}
};