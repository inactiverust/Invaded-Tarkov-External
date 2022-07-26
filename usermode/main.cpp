#include "common.hpp"
#include "memory.hpp"
#include "globals.hpp"
#include "menu.hpp"

#include "auth/skStr.h"
#include "auth/auth.hpp"

bool should_exit;

#define check_tarkov false;
#define check_auth false;
#define using_signed false;

void cheat_entry()
{
	while (!should_exit)
	{
		Sleep(1);
	}

	Sleep(40);

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
	system("CLS");
}

int main()
{
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
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)cheat_entry, 0, 0, 0);
	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)menu::render, 0, 0, 0);

	for (;;) {
		if (GetAsyncKeyState(VK_END) != 0)
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