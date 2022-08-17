#pragma once
#include "common.hpp"
#include "memory.hpp"
#include "globals.hpp"
#include "offsets.hpp"
#include "math.hpp"

#include "Physical.hpp"
#include "ProceduralWeaponAnimation.hpp"
#include "Profile.hpp"
#include "Movement.hpp"

bool null_renderer(const DWORD64 renderer)
{
	if (renderer)
		return false;

	static const int maxMaterialCount = 2;
	const auto materialCount = memory::read<int>(renderer + 0x158);
	if (materialCount <= 0 || materialCount > maxMaterialCount)
		return false;

	const auto materials = memory::read<uintptr_t>(renderer + 0x148);

	if (!materials)
		return false;

	static const std::vector<DWORD> nullVec(maxMaterialCount, 0);
	memory::write(materials, (LPVOID)nullVec.data(), materialCount * sizeof(DWORD));
		return false;

	return true;
}
/*
void Entity::WriteGearChams()t
{
	const auto slots = iUnity.ReadList<DWORD64>(iMemory.CalcPointer(this->addresses.object, iOffsets._EFTPlayer.slotViewList));

	for (const auto& slot : slots)
	{
		if (!iMemory.IsValidPointer(slot))
			continue;

		const auto dresses = iUnity.ReadArray<DWORD64>(iMemory.ReadAddress(slot + iOffsets.EFTPlayerBodyShader.Dresses));

		for (const auto& dress : dresses)
		{
			if (!iMemory.IsValidPointer(dress))
				continue;

			const auto renderers = iUnity.ReadArray<DWORD64>(iMemory.ReadAddress(dress + iOffsets.EFTVisualDress.Renderers));

			for (const auto& renderer : renderers)
			{
				if (!iMemory.IsValidPointer(renderer))
					continue;

				this->WriteNullRenderer(iMemory.ReadAddress(renderer + 0x10));
			}
		}
	}
}

DWORD64 Entity::ReadSkinnedMeshRendererFromSkin(const DWORD64 lod) const
{
	const auto className = iMemory.ReadString(iMemory.CalcPointer(lod, iOffsets.UnityObject.className));

	if (className == XOR("Skin"))
		return iMemory.CalcPointer(lod, { iOffsets.DizSkinningSkin._skinnedMeshRenderer, 0x10, 0x0 });
	else if (className == XOR("TorsoSkin"))
		return iMemory.CalcPointer(lod, { iOffsets.EFTVisualTorsoSkin._skin, iOffsets.DizSkinningSkin._skinnedMeshRenderer, 0x10, 0x0 });

	return 0;
}

void Entity::WriteSkinChams() const
{
	const auto bodySkins = iUnity.ReadDictionary(this->addresses.bodyskins);

	for (const auto& loddedSkin : bodySkins)
	{
		if (!iMemory.IsValidPointer(loddedSkin.value))
			continue;

		const auto lods = iUnity.ReadArray<DWORD64>(iMemory.ReadAddress(loddedSkin.value + iOffsets.EFTVisualLoddedSkin._lods));

		for (const auto& skin : lods)
		{
			if (!iMemory.IsValidPointer(skin))
				continue;

			const auto skinnedMeshRenderer = this->ReadSkinnedMeshRendererFromSkin(skin);
			if (!skinnedMeshRenderer)
				continue;

			// Check if player is on 2D screen https://www.unknowncheats.me/forum/3491356-post7855.html
			//const bool bool1 = iMemory.ReadValue<DWORD>(skinnedMeshRenderer + 0x128) & 0x1000;
			//const bool bool2 = iMemory.ReadValue<DWORD>(skinnedMeshRenderer + 0x168) != 0xFFFFFFFF;
			//const bool isOn2DScreen = (bool1 && bool2);

			this->WriteNullRenderer(skinnedMeshRenderer);
		}
	}
}
*/
class Player
{
private:
public:
	bool is_local_player()
	{
		return memory::read<bool>((uintptr_t)(this) + oIsLocalPlayer);
	}

	void write_skin_chams()
	{
		const auto player_body = memory::read<uintptr_t>((uintptr_t)this + 0xa8);

		const auto p_skins_dict = memory::read<uintptr_t>(player_body + 0x38);

		uint32_t SkinsCount = memory::read<uint32_t>(p_skins_dict + 0x40);

		if (!SkinsCount || SkinsCount > 10000)
			return;

		uint64_t SkinEntries = memory::read<uint64_t>(p_skins_dict + 0x18);

		for (int i = 0; i < SkinsCount; i++)
		{
			uint64_t pBodySkins = memory::read<uint64_t>(SkinEntries + 0x30 + (0x18 * i));

			if (!pBodySkins)
				continue;

			uint64_t pLodsArray = memory::read<uint64_t>(pBodySkins + 0x18);

			if (!pLodsArray)
				continue;

			uint32_t LodsCount = memory::read<uint32_t>(pLodsArray + 0x18);

			if (LodsCount > 10)
				continue;

			for (int j = 0; j < LodsCount; j++)
			{
				uint64_t pLodEntry = memory::read<uint64_t>(pLodsArray + 0x20 + (j * 0x8));

				if (j == 1)
					pLodEntry = memory::read<uint64_t>(pLodEntry + 0x20);

				uint64_t SkinnedMeshRenderer = memory::read<uint64_t>(pLodEntry + 0x20);

				if (!SkinnedMeshRenderer)
					continue;

				null_renderer(SkinnedMeshRenderer);
			}
		}
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

	Movement* get_movement()
	{
		return memory::read<Movement*>((uintptr_t)this + oMovement);
	}
};
