#include "memory.hpp"
#include "common.hpp"
#include "Aimbot.hpp"

namespace features
{
	void infinite_stamina()
	{
		if (settings::is_infinite_stamina)
		{
			pointers::local_player->get_physical()->set_stamina(100.f);
		}
	}

	void no_visor()
	{
		if (settings::is_no_visor)
		{
			uint64_t visor_effect = memory::GetComponentFromGameObject(camera.object, "VisorEffect");
			if (!visor_effect)
				return;
			memory::write<int>(visor_effect + 0xb8, 0);
		}
	}

	void thermal_vision()
	{
		if (settings::is_thermal_vision)
		{
			uint64_t thermal_effect = memory::GetComponentFromGameObject(camera.object, "ThermalVision");
			if (!thermal_effect)
				return;
			uint64_t thermal_script = memory::read<uint64_t>(thermal_effect + 0x28);
			memory::write<BYTE>(thermal_script + 0xD0, 1);
		}
	}

	void weapon_mods()
	{
		if (settings::is_no_recoil)
		{
			pointers::local_player->get_weapon()->set_no_recoil();
		}
		if (settings::is_no_spread)
		{
			pointers::local_player->get_weapon()->set_effectors();
			pointers::local_player->get_weapon()->set_mask(16);
		}
	}

	void insta_aim()
	{
		if (settings::is_instant_aim)
		{
			pointers::local_player->get_weapon()->set_aim_speed(10);
		}
	}
	void aimbot()
	{
		if (settings::is_aimbot)
		{
			Aim::find_target();
			if (GetAsyncKeyState(VK_XBUTTON2) != 0)
			{
				Aim::do_aim();
			}
		}
	}
}