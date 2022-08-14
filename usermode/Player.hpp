#pragma once
#include "common.hpp"
#include "memory.hpp"
#include "globals.hpp"
#include "offsets.hpp"
#include "math.hpp"

#include "Physical.hpp"
#include "ProceduralWeaponAnimation.hpp"
#include "Profile.hpp"
#include "Movement.hpp"

class Player
{
private:
public:
	bool is_local_player()
	{
		return memory::read<bool>((uintptr_t)(this) + oIsLocalPlayer);
	}

	Vector3 get_position(int bone_index)
	{
		return Bone::get_bone((uintptr_t)this, bone_index);
	}

	Physical* get_physical()
	{
		return memory::read<Physical*>((uintptr_t)this + oPhysical);
	}

	ProceduralWeaponAnimation* get_weapon()
	{
		return memory::read<ProceduralWeaponAnimation*>((uintptr_t)this + oWeaponAnimation);
	}

	Profile* get_profile()
	{
		return memory::read<Profile*>((uintptr_t)this + oProfile);
	}

	Movement* get_movement()
	{
		return memory::read<Movement*>((uintptr_t)this + oMovement);
	}
};
