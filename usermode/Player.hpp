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
	struct HealthStruct
	{
		float current;
		float max;
	};
public:
	bool is_local_player()
	{
		return memory::read<bool>((uintptr_t)(this) + oIsLocalPlayer);
	}

	void write_chams()
	{
		cloth_chams();
		gear_chams();
	}

	float get_health()
	{
		uintptr_t health_controller = memory::read<uintptr_t>((uintptr_t)this + oHealthController);
		uintptr_t health_body = memory::read<uintptr_t>(health_controller + oHealthBody);
		uintptr_t body_controller = memory::read<uintptr_t>(health_body + 0x18);

		float current_health = 0.f;
		float health_max = 0.f;

		for (int i = 0; i < 7; i++)
		{
			uintptr_t body_part = memory::read<uintptr_t>(body_controller + 0x30 + (i * 0x18));
			uintptr_t health_container = memory::read<uintptr_t>(body_part + 0x10);
			HealthStruct health = memory::read<HealthStruct>(health_container + 0x10);

			current_health += health.current;
			health_max += health.max;
		}

		if (health_max == 0)
			return 0;

		return (current_health / health_max);
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

private:
	void gear_chams()
	{
		if (!this)
			return;

		uintptr_t player_body = memory::read<uintptr_t>((uintptr_t)this + 0xa8);;

		if (!player_body)
			return;

		uintptr_t slot_views = memory::read<uintptr_t>(player_body + 0x50);

		if (!slot_views)
			return;

		uintptr_t slot_views_list = memory::read<uintptr_t>(slot_views + 0x18);

		if (!slot_views_list)
			return;

		uintptr_t list_pointer = memory::read<uintptr_t>(slot_views_list + 0x10);

		uint32_t sz = memory::read<uint32_t>(slot_views_list + 0x18);

		for (int i = 0; i < sz; i++)
		{
			uintptr_t entry = memory::read<uintptr_t>(list_pointer + 0x20 + (0x8 * i));

			if (!entry)
				continue;

			uintptr_t dresses_array = memory::read<uintptr_t>(entry + 0x40);

			if (!dresses_array)
				continue;

			uint32_t dress_arr_sz = memory::read<uintptr_t>(dresses_array + 0x18);

			for (int j = 0; j < dress_arr_sz; j++)
			{
				uintptr_t dress_entry = memory::read<uintptr_t>(dresses_array + 0x20 + (0x8 * j));

				if (!dress_entry)
					continue;

				uintptr_t renderer_array = memory::read<uintptr_t>(dress_entry + 0x28);

				if (!renderer_array)
					continue;

				uint32_t render_arr_sz = memory::read<uint32_t>(renderer_array + 0x18);

				for (int k = 0; k < render_arr_sz; k++)
				{
					uintptr_t render_entry = memory::read<uintptr_t>(renderer_array + 0x20 + (0x8 * k));

					if (!render_entry)
						continue;

					uintptr_t material_dict = memory::read<uintptr_t>(render_entry + 0x10);

					if (!material_dict)
						continue;

					uint32_t mat_count = memory::read<uint32_t>(material_dict + 0x158);

					if (mat_count > 0 && mat_count < 6) // rofl
					{
						uintptr_t material_dictionary_base = memory::read<uintptr_t>(material_dict + 0x148);

						if (!material_dictionary_base)
							continue;

						for (int l = 0; l < mat_count; l++)
							memory::write<DWORD>(material_dictionary_base + (l * 0x4), 0);
					}
				}
			}
		}
	}

	void cloth_chams()
	{
		if (!this)
			return;

		uintptr_t player_body = memory::read<uintptr_t>((uintptr_t)this + 0xa8);;

		if (!player_body)
			return;

		uintptr_t skin_dictionary = memory::read<uintptr_t>(player_body + 0x38);

		if (!skin_dictionary)
			return;

		uint32_t skin_count = memory::read<uint32_t>(skin_dictionary + 0x40);

		uintptr_t skin_list = memory::read<uintptr_t>(skin_dictionary + 0x18);

		if (!skin_count || skin_count > 10000)
			return;

		for (int i = 0; i < skin_count; i++)
		{
			uintptr_t body_skins = memory::read<uintptr_t>(skin_list + 0x30 + (0x18 * i));

			if (!body_skins)
				continue;

			uintptr_t lod_array = memory::read<uintptr_t>(body_skins + 0x18);

			if (!lod_array)
				continue;

			int lod_count = memory::read<int>(lod_array + 0x18);

			if (lod_count > 128)
				continue;

			for (int j = 0; j < lod_count; j++)
			{

				uintptr_t lod_entry = memory::read<uintptr_t>(lod_array + 0x20 + (j * 0x8));
				if (!lod_entry)
					continue;

				uintptr_t skinned_mesh_renderer = memory::read<uintptr_t>(lod_entry + 0x20);
				if (!skinned_mesh_renderer)
					continue;

				uintptr_t material_dict = memory::read<uintptr_t>(skinned_mesh_renderer + 0x10);
				if (!material_dict)
					continue;

				uint32_t material_count = memory::read<uint32_t>(material_dict + 0x158);

				if (material_count > 0 && material_count < 10)
				{
					uintptr_t material_dirbase = memory::read<uintptr_t>(material_dict + 0x148);

					for (int k = 0; k < material_count; k++)
					{
						if (memory::read<uint64_t>(material_dirbase + (k * 0x4)) != 0)
							memory::write<uint64_t>(material_dirbase + (k * 0x4), 0);
					}
				}
				else
				{
					uintptr_t lod_entry = memory::read<uint64_t>(lod_array + 0x20 + (j * 0x8));
					if (!lod_entry)
						continue;

					uintptr_t skinned_mesh_renderer = memory::read<uintptr_t>(memory::read<uint64_t>(lod_entry + 0x20) + 0x20);
					if (!skinned_mesh_renderer)
						continue;

					uintptr_t material_dict = memory::read<uintptr_t>(skinned_mesh_renderer + 0x10);
					if (!material_dict)
						continue;

					uint32_t material_count = memory::read<uint32_t>(material_dict + 0x158);
					uintptr_t material_dirbase = memory::read<uintptr_t>(material_dict + 0x148);
					for (int k = 0; k < material_count; k++)
					{
						if (memory::read<uint64_t>(material_dirbase + (k * 0x4)) != 0)
							memory::write<uint64_t>(material_dirbase + (k * 0x4), 0);
					}
				}
			}
		}
	}
};

class LPlayer
{
public:
	void setup()
	{
		weapon_class = player_class->get_weapon();
		physical_class = player_class->get_physical();

		position = player_class->get_position(Bone::bones::HumanHead);
		fireport_position = weapon_class->get_fireport();
		
	}

	Player* player_class;
	ProceduralWeaponAnimation* weapon_class;
	Physical* physical_class;
	Vector3 position;
	Vector3 fireport_position;
} local_player;
