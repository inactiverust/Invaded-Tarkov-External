#pragma once

#include "common.hpp"
#include "memory.hpp"
#include "offsets.hpp"
#include "Bone.hpp"

class ProceduralWeaponAnimation
{
private:
public:
	void set_no_recoil()
	{
		uintptr_t shot_effector = memory::read<uintptr_t>((uintptr_t)this + oShooting);
		memory::write<float>(shot_effector + oIntensity, 0.f);
	}

	Vector3 get_fireport()
	{
		if (!reinterpret_cast<uintptr_t>(this))
			return {};

		uintptr_t fire_arm_controller = memory::read<uintptr_t>((uintptr_t)this + oFireArmController);
		uintptr_t transform = memory::read<uintptr_t>(fire_arm_controller + 0x88);
		transform = memory::read<uintptr_t>(transform + 0x10);
		return Bone::get_transform_position(transform);
	}

	void set_aim_speed(float speed)
	{
		memory::write<float>((uintptr_t)this + oAimSpeed, speed);
	}
};
