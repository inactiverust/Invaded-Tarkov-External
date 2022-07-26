#pragma once

#include "common.hpp"
#include "GameObjectManager.hpp"
#include "World.hpp"
#include "Player.hpp"

namespace misc
{
}

namespace vars
{
	uint32_t target_pid;
	std::vector<uintptr_t> players_list{};
}

namespace pointers
{
	uintptr_t unity_player;
	GameObjectManager* GOM;
	World* world;
	Player* local_player;
}

namespace settings {
}

namespace keybinds
{
}