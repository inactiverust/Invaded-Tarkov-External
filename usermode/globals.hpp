#pragma once 

#include "common.hpp"
#include "memory.hpp"
#include "GameObjectManager.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Camera.hpp"

namespace vars
{
	uint32_t target_pid;
	uint32_t draw_pid;
	Player* aim_player;
	std::vector<uintptr_t> players_list{};
}

namespace pointers
{
	uintptr_t unity_player;
	GameObjectManager* GOM;
	World* world;
	Camera* camera;
	Player* local_player;
}

namespace settings 
{
	bool connected;
	bool is_in_raid;
	bool is_aimbot;
	bool is_infinite_stamina;
	bool is_no_recoil;
	bool is_no_spread;
}

namespace keybinds
{
}