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

			if (current == local_player.player_class)
				continue;
			
			Vector2 ScreenPos = {};
			Vector3 PlayerPos = current->get_position(Bone::bones::HumanHead);

			if (!world_to_screen(PlayerPos, ScreenPos))
				continue;

			float fov = Calc2D_Dist(Vector2(ScreenCenterX, ScreenCenterY), ScreenPos);
			float distance = Calc3D_Dist(PlayerPos, local_player.position);

			if (fov < min_fov && fov < settings::aim::aim_fov && distance < settings::aim::max_distance)
			{
				min_fov = fov;
				return_player = current;
			}
		}
		vars::aim_player = return_player;
	}

	void do_aim()
	{
		if (!vars::aim_player)
			return;
		
		Vector3 AimPos = vars::aim_player->get_position(settings::aim::aim_bone);
		Vector3 PlayerPos = local_player.fireport_position;

		Vector2 Angles = calculate_angle(PlayerPos, AimPos);

		if (Angles.x == 0 || Angles.y == 0 || isnan(Angles.x) || isnan(Angles.y))
		{
			return;
		}

		local_player.player_class->get_movement()->set_va(Angles);
	}
}