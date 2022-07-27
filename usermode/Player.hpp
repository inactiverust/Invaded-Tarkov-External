#pragma once
#include "common.hpp"
#include "memory.hpp"
#include "offsets.hpp"

#include "Physical.hpp"
#include "ProceduralWeaponAnimation.hpp"

class Player
{
private:
public:
	bool is_local_player()
	{
		return memory::read<bool>((uintptr_t)(this) + oIsLocalPlayer);
	}

	Physical* get_physical()
	{
		return memory::read<Physical*>((uintptr_t)this + oPhysical);
	}

	ProceduralWeaponAnimation* get_weapon()
	{
		return memory::read<ProceduralWeaponAnimation*>((uintptr_t)this + oWeaponAnimation);
	}

};
