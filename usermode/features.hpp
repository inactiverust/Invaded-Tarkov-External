#include "memory.hpp"
#include "common.hpp"
#include "Aimbot.hpp"
#include <tuple>

namespace features
{
	void infinite_stamina()
	{
		if (settings::is_infinite_stamina)
		{
			local_player.physical_class->set_stamina(100.f);
		}
	}

	void no_visor()
	{
		if (settings::is_no_visor)
		{
			uint64_t visor_effect = memory::GetComponentFromGameObject(camera.object, "VisorEffect");
			//if (!visor_effect)
				//return;
			//memory::write<int>(visor_effect + 0xb8, 0);
		}
	}

	void do_cham()
	{
		if (settings::is_chams)
		{
			for (auto& player : vars::players_list)
			{
				Player* current = (Player*)player;
				if (current != local_player.player_class)
				{
					current->write_chams();
				}
			}
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
			local_player.weapon_class->set_no_recoil();
		}
		if (settings::is_no_spread)
		{
			local_player.weapon_class->set_effectors();
			local_player.weapon_class->set_mask(16);
		}
		else
		{
			local_player.weapon_class->set_mask(127);
		}
	}

	void insta_aim()
	{
		if (settings::is_instant_aim)
		{
			local_player.weapon_class->set_aim_speed(10);
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

	void loot_esp()
	{
		if (settings::esp::is_loot_esp)
		{
			std::vector<WorldLoot> temp;
			for (auto& loot : loot_list.loot_pointer_list)
			{
				bool cached = false;

				LootItem* current = (LootItem*)loot;

				WorldLoot item;

				Vector3 position = current->get_position();

				float distance = Calc3D_Dist(position, local_player.position);

				if (distance > settings::esp::loot_esp::max_distance)
					continue;

				Vector2 ScreenPos;
				if (!world_to_screen(position, ScreenPos))
					continue;

				std::string id = current->get_name();
				std::string shortname;
				uint32_t price;

				if (id == "Corpse")
				{
					item.short_name = "Corpse";
					item.location = ScreenPos;
					item.price = 0;
					item.distance = (int)distance;
					temp.push_back(item);
					continue;
				}

				for (auto& entry : cached_item_list)
				{
					if (entry.id == id)
					{
						cached = true;
						shortname = entry.shortname;
						price = entry.price;
					}
				}

				if (!cached)
				{
					if (!LootItem::query_item_id(vars::loot_values, id, shortname, price))
						continue;
				}

				if (price < settings::esp::loot_esp::min_price)
					continue;

				item.price = price;
				item.short_name = shortname;
				item.location = ScreenPos;
				item.id = id;
				item.distance = (int)distance;
				temp.push_back(item);
			}
			loot_list.world_loot_list = std::move(temp);
		}
	}
	void loot_thru_walls()
	{
		if (settings::is_loot_thru_walls)
		{
			static auto eft_hard_settings = memory::read<uint64_t>(mono::find_class(("Assembly-CSharp"), ("EFTHardSettings"))->get_vtable(mono::get_root_domain())->get_static_field_data());
			if (eft_hard_settings)
				if (memory::read<float>(eft_hard_settings + 0x1d4) != 4.f)
					memory::write<float>(eft_hard_settings + 0x1d4, 4.f);

			static auto game_world_instance = mono::find_class(("Assembly-CSharp"), ("EFT.GameWorld"))->get_vtable(mono::get_root_domain())->get_static_field_data();
			if (game_world_instance)
				if (memory::read<int>(game_world_instance + 0xc) != 0)
					memory::write<int>(game_world_instance + 0xc, 0);
		}
	}
	void extract_esp()
	{
		if (settings::esp::show_extracts)
		{
			std::vector<Extract_Info> temp;
			for (auto& extract : exfil_list.exfil_pointer_list)
			{
				ExfiltrationPoint* current = (ExfiltrationPoint*)extract;

				Vector3 pos = current->get_position();
				Vector2 screen_pos;

				float distance = Calc3D_Dist(pos, local_player.position);

				if (!world_to_screen(pos, screen_pos))
					continue;

				Extract_Info info;
				info.name = current->get_name();
				info.distance = distance;
				info.screen_pos = screen_pos;
				temp.push_back(info);
			}
			exfil_list.extract_info_list = std::move(temp);
		}
		else
		{
			exfil_list.extract_info_list.clear();
		}
	}

	void esp()
	{
		if (settings::is_esp)
		{
			std::vector<Draw_Info> temp_list;

			for (auto& player : vars::players_list)
			{
				Player* current = (Player*)player;

				if (current == local_player.player_class)
					continue;

				Vector3 HeadPosition = current->get_position(Bone::bones::HumanHead);
				HeadPosition.y += 0.25;
				Vector3 BasePosition = (current->get_position(Bone::bones::HumanLFoot) + current->get_position(Bone::bones::HumanRFoot)) / 2;

				Vector2 HeadScreenPosition;
				Vector2 BaseScreenPosition;

				if (!world_to_screen(HeadPosition, HeadScreenPosition) || !world_to_screen(BasePosition, BaseScreenPosition))
					continue;

				Draw_Info info;
				Player_Info playerInfo;

				playerInfo.type = current->get_profile()->get_role();
				playerInfo.distance = (int)Calc3D_Dist(local_player.position, HeadPosition);
				if (playerInfo.distance > settings::esp::max_distance)
					continue;

				if (settings::esp::show_health)
					playerInfo.health = current->get_health();

				info.Head_Position = HeadScreenPosition;
				info.Base_Position = BaseScreenPosition;
				info.p_info = playerInfo;

				temp_list.push_back(info);
			}
			vars::drawing_list = std::move(temp_list);

			if (settings::esp::show_aim_line && vars::aim_player)
			{
				Vector3 AimPos = vars::aim_player->get_position(settings::aim::aim_bone);
				Vector3 PlayerPos = local_player.fireport_position;

				Vector2 AimSPos;
				Vector2 PlayerSPos;

				world_to_screen(AimPos, AimSPos);
				world_to_screen(PlayerPos, PlayerSPos);

				vars::target_info.FirePortPos = PlayerSPos;
				vars::target_info.PlayerPos = AimSPos;
			}
			else
			{
				vars::target_info.FirePortPos = {};
				vars::target_info.PlayerPos = {};
			}
		}
	}
}