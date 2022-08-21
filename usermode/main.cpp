#include "common.hpp"
#include "memory.hpp"

#include "auth/skStr.h"
#include "auth/auth.hpp"

#include "drawing.hpp"
#include "features.hpp"
#include "drawing.hpp"

bool should_exit;
bool do_once = true;

#define check_tarkov false
#define check_auth false
#define using_signed false

void update_drawing_list()
{
	std::vector<Draw_Info> temp_list;

	Vector3 LocalPosition = pointers::local_player->get_position(Bone::bones::HumanHead);

	for (auto& player : vars::players_list)
	{
		Player* current = (Player*)player;

		if (current == pointers::local_player)
			continue;
		
		Vector3 HeadPosition = current->get_position(Bone::bones::HumanHead);
		HeadPosition.y += 0.5;
		Vector3 BasePosition = (current->get_position(Bone::bones::HumanLFoot) + current->get_position(Bone::bones::HumanRFoot)) / 2;
		BasePosition.y += 0.2;

		Vector2 HeadScreenPosition;
		Vector2 BaseScreenPosition;

		if (!world_to_screen(HeadPosition, HeadScreenPosition) || !world_to_screen(BasePosition, BaseScreenPosition))
			continue;

		Draw_Info info;
		Player_Info playerInfo;

		playerInfo.type = current->get_profile()->get_role();
		playerInfo.distance = (int)Calc3D_Dist(LocalPosition, HeadPosition);

		info.Head_Position = HeadScreenPosition;
		info.Base_Position = BaseScreenPosition;
		info.p_info = playerInfo;

		temp_list.push_back(info);
	}
	vars::drawing_list = std::move(temp_list);
}

void update_player_list()
{
	Player* return_player = NULL;
	pointers::world = (World*)pointers::GOM->get_game_world();
	if (pointers::world)
	{
		vars::players_list = pointers::world->get_player_list();

		if (vars::players_list.size() > 0)
		{
			for (auto& player : vars::players_list)
			{
				Player* current = (Player*)player;
				if (current->is_local_player())
				{
					return_player = current;
					break;
				}
			}
		}
	}
	pointers::local_player = return_player;
}

void cheat_entry()
{
	while (!should_exit)
	{
		Sleep(1);
	}

	Sleep(40);

	pointers::GOM = memory::read<GameObjectManager*>(pointers::unity_player + oGOM);

	if(!pointers::GOM)
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		std::cout << _("Error 2");
		Sleep(3000);
		exit(3);
	}

	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();

	while (true)
	{
		update_player_list();
		if (pointers::local_player)
		{
			if (settings::is_in_raid)
			{
				camera.object = pointers::GOM->get_fps_camera();
				features::infinite_stamina();
				features::weapon_mods();
				features::aimbot();
				features::insta_aim();
				features::no_visor();
				features::thermal_vision();

				auto stop = std::chrono::high_resolution_clock::now();
				if (std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() > 10)
				{
					start = std::chrono::high_resolution_clock::now();
					features::do_cham();
				}

				if (settings::is_esp)
					update_drawing_list();
				else
					vars::drawing_list.clear();
			}
			else
			{
				vars::drawing_list.clear();
			}
		}
	}
}

void load_drv()
{
	std::string path = std::filesystem::current_path().string();
	auth::download_file("206642", "invdriver.sys");
	system(("sc create invaded type= kernel binPath= " + path + "\\invdriver.sys").c_str());
	system("sc start invaded");
	Sleep(300);
	system("sc stop invaded");
	remove("invdriver.sys");
	system("sc delete invaded");
	//system("CLS");
}

int main()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)cheat_entry, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)draw::Initialize, 0, 0, 0);

	ScreenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2;
	ScreenCenterY = GetSystemMetrics(SM_CYSCREEN) / 2;

#if check_tarkov
	if (memory::get_pid(_("EscapeFromTarkov.exe")))
	{
		std::cout << _("Close EscapeFromTarkov . . .");
		Sleep(3000);
		exit(3);
	}
#endif

#if check_auth
	auth::do_auth();
#endif

#if check_tarkov
	if (memory::get_pid(_("EscapeFromTarkov.exe")))
	{
		std::cout << _("Close EscapeFromTarkov . . .");
		Sleep(3000);
		exit(3);
	}
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

#if using_signed
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)load_drv, 0, 0, 0);
#endif;

	Sleep(1000);

	for (;;) {
		if (GetAsyncKeyState(VK_INSERT) != 0)
			break;
		Sleep(1);
	}

	vars::target_pid = memory::get_pid(_("EscapeFromTarkov.exe"));

	pointers::unity_player = memory::find_base_address(vars::target_pid, _(L"UnityPlayer.dll"));

	if (!pointers::unity_player)
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		std::cout << _("Error 1");
		Sleep(3000);
		exit(3);
	}

	memory::setup(vars::target_pid);

	should_exit = true;

	memory::loop();
}