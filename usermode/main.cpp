#include "common.hpp"
#include "memory.hpp"

#include "auth/skStr.h"
#include "auth/auth.hpp"

#include "drawing.hpp"
#include "features.hpp"

bool should_exit;

#define check_tarkov false
#define check_auth false
#define using_signed false

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

namespace drawing
{
	uintptr_t input_pointer = 140698862838648;

	enum OperationType
	{
		finished,
		line,
		clear
	};

	struct Draw_Info
	{
		Vector2 head_pos;
		int operation;
	};

	void push_esp()
	{
		std::vector<Draw_Info> temp_list;
		for (auto& player : vars::players_list)
		{
			Player* current = (Player*)player;
			if (current == pointers::local_player)
				continue;
			Vector3 position = current->get_position(Bone::bones::HumanHead);
			Vector2 ScreenPos;

			world_to_screen(position, ScreenPos);

			Draw_Info list_entry;
			list_entry.head_pos = ScreenPos;
			list_entry.operation = OperationType::line;
			temp_list.push_back(list_entry);
		}

		Draw_Info clear;
		clear.operation = OperationType::clear;

		memory::d_write(input_pointer, clear);

		for (auto& entry : temp_list)
		{
			memory::d_write(input_pointer, entry);
		}
	}
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
				std::cout << "list size: " << vars::players_list.size() << "\n";
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
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)menu::render, 0, 0, 0); 
	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)menu::draw_overlay, 0, 0, 0);

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
		if (settings::connected == true)
			break;
		Sleep(1);
	}

	vars::target_pid = memory::get_pid(_("EscapeFromTarkov.exe"));
	vars::draw_pid = memory::get_pid(_("Cheat.exe"));

	pointers::unity_player = memory::find_base_address(vars::target_pid, _(L"UnityPlayer.dll"));

	if (!pointers::unity_player)
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		std::cout << _("Error 1");
		Sleep(3000);
		exit(3);
	}

	memory::setup(vars::target_pid);
	memory::d_setup(vars::draw_pid);

	should_exit = true;

	memory::loop();
}