#pragma once

#include "common.hpp"
#include "memory.hpp"
#include "Camera.hpp"
#include "Player.hpp"

namespace Aim
{
	void find_target()
	{
		Player* return_player = NULL;
		float min_fov = 9999.f;
		for (auto& player : vars::players_list)
		{
			Player* current = (Player*)player;

			if (current == pointers::local_player)
				continue;
			
			Vector2 ScreenPos = {};
			Vector3 PlayerPos = current->get_position(Bone::bones::HumanPelvis);

			std::cout << PlayerPos.x << " " << PlayerPos.y << " " << PlayerPos.z << "\n";

			if (!world_to_screen(PlayerPos, ScreenPos))
				continue;

			float fov = Calc2D_Dist(Vector2(ScreenCenterX, ScreenCenterY), ScreenPos);
			std::cout << ScreenPos.x << " " << ScreenPos.y << "\n";

			if (fov < min_fov);
			{
				return_player = current;
				min_fov = fov;
			}
		}
		vars::aim_player = return_player;
	}

	void do_aim()
	{
		if (!vars::aim_player)
			return;
		
		Vector3 AimPos = vars::aim_player->get_position(Bone::bones::HumanHead);
		Vector3 PlayerPos = pointers::local_player->get_weapon()->get_fireport();

		Vector2 Angles = calculate_angle(PlayerPos, AimPos);

		if (Angles.x == 0 || Angles.y == 0 || isnan(Angles.x) || isnan(Angles.y))
		{
			return;
		}

		pointers::local_player->get_movement()->set_va(Angles);
	}
}