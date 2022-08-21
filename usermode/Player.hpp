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

	void write_chams()
	{
		cloth_chams();
		gear_chams();
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

			if (lod_count > 10000)
				continue;

			for (int j = 0; j < lod_count; j++)
			{
			
				uintptr_t lod_entry = memory::read<uintptr_t>(lod_array + 0x20 + (j * 0x8));

				if (j == 1)
					lod_entry = memory::read<uintptr_t>(lod_entry + 0x20);

				uintptr_t skinned_mesh_render = memory::read<uintptr_t>(lod_entry + 0x20);

				if (!skinned_mesh_render)
					continue;

				uintptr_t material_dictionary = memory::read<uintptr_t>(skinned_mesh_render + 0x10);

				if (!material_dictionary)
					continue;

				uint32_t mat_count = memory::read<uint32_t>(material_dictionary + 0x158);

				if (mat_count > 0 && mat_count < 5)
				{
					uintptr_t material_dictionary_base = memory::read<uintptr_t>(material_dictionary + 0x148);

					if (!material_dictionary_base)
						continue;

					for (int l = 0; l < mat_count; l++)
						memory::write<DWORD>(material_dictionary_base + (l * 0x4), 0);
				}
			}
		}
	}
};
