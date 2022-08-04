#pragma once
#include "common.hpp"
#include "memory.hpp"
#include "offsets.hpp"
#include "math.hpp"

#include "Physical.hpp"
#include "ProceduralWeaponAnimation.hpp"
#include "Profile.hpp"

namespace Bone
{
	struct transform_access_read_only_t
	{
		uint64_t transform_data{};
	};

	struct transform_data_t
	{
		uint64_t transform_array{};
		uint64_t transform_indices{};
	};

	struct matrix34_t
	{
		Vector4 vec0{};
		Vector4 vec1{};
		Vector4 vec2{};
	};

	Vector3 get_transform_position(uintptr_t pTransform)
	{
		__m128 result{};

		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		transform_access_read_only_t transform_access_read_only = memory::read<transform_access_read_only_t>(pTransform + 0x38);
		unsigned int index = memory::read<unsigned int>(pTransform + 0x40);
		transform_data_t transform_data = memory::read<transform_data_t>(transform_access_read_only.transform_data + 0x18);

		if (transform_data.transform_array && transform_data.transform_indices)
		{
			result = memory::read<__m128>(transform_data.transform_array + 0x30 * index);
			int transform_index = memory::read<int>(transform_data.transform_indices + 0x4 * index);
			int safe = 0;
			while (transform_index >= 0 && safe++ < 200)
			{
				matrix34_t matrix = memory::read<matrix34_t>(transform_data.transform_array + 0x30 * transform_index);

				__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x00));	// xxxx
				__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x55));	// yyyy
				__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x8E));	// zwxy
				__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0xDB));	// wzyw
				__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0xAA));	// zzzz
				__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x71));	// yxwy
				__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix.vec2), result);

				result = _mm_add_ps(_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), *(__m128*)(&matrix.vec0));

				transform_index = memory::read<int>(transform_data.transform_indices + 0x4 * transform_index);
			}
		}


		return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}

	Vector3 get_bone(uintptr_t player, int bone_index)
	{
		uintptr_t bone = memory::read<uintptr_t>(player + oPlayerBody);
		bone = memory::read<uintptr_t>(bone + 0x28);
		bone = memory::read<uintptr_t>(bone + 0x28);
		bone = memory::read<uintptr_t>(bone + 0x10);
		bone = memory::read<uintptr_t>(bone + (0x20 + (bone_index * 0x8)));
		bone = memory::read<uintptr_t>(bone + 0x10);
		return get_transform_position(bone);
	}
}
class Player
{
private:
public:
	enum bones : int
	{
		HumanBase = 0,
		HumanPelvis = 14,
		HumanLThigh1 = 15,
		HumanLThigh2 = 16,
		HumanLCalf = 17,
		HumanLFoot = 18,
		HumanLToe = 19,
		HumanRThigh1 = 20,
		HumanRThigh2 = 21,
		HumanRCalf = 22,
		HumanRFoot = 23,
		HumanRToe = 24,
		HumanSpine1 = 29,
		HumanSpine2 = 36,
		HumanSpine3 = 37,
		HumanLCollarbone = 89,
		HumanLUpperarm = 90,
		HumanLForearm1 = 91,
		HumanLForearm2 = 92,
		HumanLForearm3 = 93,
		HumanLPalm = 94,
		HumanRCollarbone = 110,
		HumanRUpperarm = 111,
		HumanRForearm1 = 112,
		HumanRForearm2 = 113,
		HumanRForearm3 = 114,
		HumanRPalm = 115,
		HumanNeck = 132,
		HumanHead = 133
	};

	bool is_local_player()
	{
		return memory::read<bool>((uintptr_t)(this) + oIsLocalPlayer);
	}

	Vector3 get_position(int bone_index)
	{
		return Bone::get_bone((uintptr_t)this, bone_index);
	}

	Physical* get_physical()
	{
		return memory::read<Physical*>((uintptr_t)this + oPhysical);
	}

	ProceduralWeaponAnimation* get_weapon()
	{
		return memory::read<ProceduralWeaponAnimation*>((uintptr_t)this + oWeaponAnimation);
	}

	Profile* get_profile()
	{
		return memory::read<Profile*>((uintptr_t)this + oProfile);
	}
};
