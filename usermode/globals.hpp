#pragma once 

#include "common.hpp"
#include "memory.hpp"
#include "GameObjectManager.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include "ExfiltrationPoint.hpp"

struct Player_Info
{
	std::string type;
	int distance;
	float health;
};

struct Draw_Info
{
	Vector2 Head_Position;
	Vector2 Base_Position;
	Player_Info p_info;
};

struct Aim_Target_Info
{
	Vector2 FirePortPos;
	Vector2 PlayerPos;
};

namespace vars
{
	uint32_t target_pid;
	uint32_t draw_pid;
	Player* aim_player;
	Aim_Target_Info target_info;
	std::vector<uintptr_t> players_list{};
	std::vector<uintptr_t> extract_list{};
	std::vector<Draw_Info> drawing_list;
}

namespace pointers
{	
	Matrix4x4* view_matrix_pointer;
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
	bool is_no_visor;
	bool is_thermal_vision;
	bool is_chams;
	bool is_esp;
	bool is_aimbot;
	bool is_infinite_stamina;
	bool is_no_recoil;
	bool is_no_spread;
	bool is_instant_aim;
	namespace aim
	{
		float aim_fov = 300.f;
		bool enable_fov_circle;
	}
	namespace esp
	{
		bool show_aim_line;
		bool show_role;
		bool show_distance;
		bool show_health;
	}
}

namespace keybinds
{
}