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

	void weapon_mods()
	{
		if (settings::is_no_spread && settings::is_no_recoil)
		{
			pointers::local_player->get_weapon()->set_effectors();
			pointers::local_player->get_weapon()->set_mask(0);
		}
		else if (settings::is_no_spread)
		{
			pointers::local_player->get_weapon()->set_effectors();
			pointers::local_player->get_weapon()->set_mask(16);
		}
		else if (settings::is_no_recoil)
		{
			pointers::local_player->get_weapon()->set_mask(1);
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