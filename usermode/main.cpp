#include "common.hpp"
#include "memory.hpp"

#include "auth/skStr.h"
#include "auth/auth.hpp"

#include "drawing.hpp"
#include "features.hpp"
#include "drawing.hpp"
//c
bool should_exit;
bool do_once = true;

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
	local_player.player_class = return_player;
}

void cheat_entry()
{
	std::ifstream file; file.open("loot.json"); std::stringstream ss; ss << file.rdbuf(); vars::loot_values = ss.str();
	while (!should_exit)
	{
		Sleep(1);
	}

	Sleep(40);

	pointers::GOM = memory::read<GameObjectManager*>(pointers::unity_player + oGOM);
	mono::init_functions();

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
		if (settings::is_in_raid)
		{
			update_player_list();
			exfil_list.exfil_pointer_list = pointers::world->get_extract_list();
			if (exfil_list.exfil_pointer_list.size() > 0 && local_player.player_class)
			{
				loot_list.loot_pointer_list = pointers::world->get_loot_list();

				camera.object = pointers::GOM->get_fps_camera();
				local_player.setup();

				features::infinite_stamina();
				features::weapon_mods();
				features::aimbot();
				features::insta_aim();
				features::loot_esp();
				features::extract_esp();
				features::loot_thru_walls();
				features::thermal_vision();
				features::esp();


				auto stop = std::chrono::high_resolution_clock::now();
				if (std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() > 10)
				{
					start = std::chrono::high_resolution_clock::now();
					features::do_cham();
				}
			}
			else
			{
				vars::drawing_list.clear();
				exfil_list.extract_info_list.clear();
				loot_list.world_loot_list.clear();
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
	mono_dll_address = memory::find_base_address(vars::target_pid, _(L"mono-2.0-bdwgc.dll"));

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