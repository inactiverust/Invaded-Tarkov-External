#include "common.hpp"
#include "math.hpp"
#include "memory.hpp"

class Movement
{
public:
	Vector2 get_va()
	{
		return memory::read<Vector2>((uintptr_t)this + 0x22c);
	}

	void set_va(Vector2 angles)
	{
		memory::write<Vector2>((uintptr_t)this + 0x22c, angles);
	}
};