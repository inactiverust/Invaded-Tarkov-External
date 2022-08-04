#pragma once
#include "common.hpp"
#include "memory.hpp"
#include "offsets.hpp"

class Profile
{
private:
public:
	std::string get_name()
	{
		
	}

	std::string get_role()
	{
		uintptr_t player_info = memory::read<uintptr_t>((uintptr_t)this + oInfo);

		uintptr_t side = memory::read<uintptr_t>((uintptr_t)this + oSide);

		uint32_t player_side = memory::read<uint32_t>(side + 0x10);

		if (memory::read<uint32_t>(player_info + oRegDate) <= 0)
		{
            switch (player_side)
            {
            case 1:
                return _("Sniper Scav");
                break;
            case 2:
                return _("Scav");
                break;
            case 8:
                return _("Reshala");
                break;
            case 16:
                return _("Follower");
                break;
            case 32:
                return _("Reshala Demon");
                break;
            case 64:
                return _("Killa");
                break;
            case 128:
                return _("Shturman");
                break;
            case 256:
                return _("Shturman Demon");
                break;
            case 512:
                return _("Raider");
                break;
            case 1024:
                return _("Cultist");
                break;
            case 2048:
                return _("Gluhar");
                break;
            case 4096:
                return _("Gluhar Demon");
                break;
            case 8192:
                return _("Gluhar Demon");
                break;
            case 16384:
                return _("Gluhar Demon");
                break;
            case 32768:
                return _("Gluhar Demon");
                break;
            case 65536:
                return _("Sanitar Demon");
                break;
            case 131072:
                return _("Sanitar");
                break;
            case 524288:
                return _("Raider");
                break;
            case 4194304:
                return _("Tagilla");
                break;
            }
		}
		else if(side == 4)
		{
            return "Player Scav";
		}
		else
		{
			return "Player";
		}
	}
};
