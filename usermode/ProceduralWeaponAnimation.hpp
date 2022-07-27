#pragma once
#include "common.hpp"
#include "memory.hpp"
#include "offsets.hpp"

class ProceduralWeaponAnimation
{
private:
public:
	void set_no_recoil()
	{
		uintptr_t shot_effector = memory::read<uintptr_t>((uintptr_t)this + oShooting);
		memory::write<float>(shot_effector + oIntensity, 0.f);
	}
	void set_aim_speed(float speed)
	{
		memory::write<float>((uintptr_t)this + oAimSpeed, speed);
	}
};
