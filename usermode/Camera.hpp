#pragma once

#include "common.hpp"
#include "memory.hpp"
#include "math.hpp"
#include "drawing.hpp"

float ScreenCenterX = 0;
float ScreenCenterY = 0;

struct vmatrix_t
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};
};

class Camera
{
public:
	uintptr_t object = NULL;
	Vector3 position{};
	vmatrix_t vmatrix{};
} camera;

bool world_to_screen(const Vector3& world, Vector2& screen)
{
	if (!memory::read<uintptr_t>(camera.object + 0xDC))
		return false;

	vmatrix_t vmatrix = memory::read<vmatrix_t>(camera.object + 0xDC /*0x2E4*/);

	Vector3 trans_vec{ vmatrix._14, vmatrix._24, vmatrix._34 };
	Vector3 right_vec{ vmatrix._11, vmatrix._21, vmatrix._31 };
	Vector3 up_vec{ vmatrix._12, vmatrix._22, vmatrix._32 };

	float w = Dot(trans_vec, world); + vmatrix._44;

	if (w < 0.098f) return false;

	float y = Dot(up_vec, world) + vmatrix._42;
	float x = Dot(right_vec, world) + vmatrix._41;

	Vector2 temp = Vector2((ScreenCenterX) * (1.f + x / w), (ScreenCenterY) * (1.f - y / w));

	if (temp.x < 0 || temp.y < 0 || temp.x > ScreenCenterX * 2 || temp.y > ScreenCenterY * 2)
		return false;

	screen = temp;
	return true;
}