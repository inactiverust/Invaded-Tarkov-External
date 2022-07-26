#pragma once
#include "common.hpp"
#include "memory.hpp"

class Player
{
private:
public:
	bool is_local_player()
	{
		return memory::read<bool>(reinterpret_cast<uintptr_t>(this) + 0x807);
	}

	bool is_dead()
	{
		return memory::read<bool>(reinterpret_cast<uintptr_t>(this) + 0x6A0);
	}
};
