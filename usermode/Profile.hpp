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
        uintptr_t info = memory::read<uintptr_t>((uintptr_t)this + oInfo);

        uintptr_t name_ptr = memory::read<uintptr_t>(info + 0x10);

        int name_sz = memory::read<int>(name_ptr + 0x10);

        return memory::get_unicode_str(name_ptr, name_sz);
	}

	std::string get_role()
	{
		uintptr_t player_info = memory::read<uintptr_t>((uintptr_t)this + oInfo);

        uint32_t registration_date = memory::read<uint32_t>(player_info + oRegistrationDate);
       
        uint32_t side = memory::read<uint32_t>(player_info + oSide);

		uintptr_t settings = memory::read<uintptr_t>(player_info + oSettings);

        uint32_t role = memory::read<uint32_t>(settings + 0x10);

        if (registration_date > 0 && (side == 1 || side == 2))
            return this->get_name();
		else if (registration_date > 0 && side == 4)
			return _("Player Scav");
		else
		{
			switch (role)
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
            default:
                return _("Undefined");
                break;
			}
		}
	}
};
