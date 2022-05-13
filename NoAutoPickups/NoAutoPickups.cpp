#include "plugin.h"
#include "CPickups.h"
#include "CGameLogic.h"
#include "CText.h"
#include "CHud.h"
#include "..\injector\assembly.hpp"

using namespace plugin;
using namespace injector;

class NoAutoPickups {
public:
	inline static bool displayHelp;

    NoAutoPickups() {

		// Make weapon pickup always be considered invalid to pick
		WriteMemory<uint32_t>(0x457CAB + 1, 0x7FFFFFFF, true);

		Events::initScriptsEvent += []() {
			displayHelp = true;
		};

		//CPickup::Update
		MakeInline<0x457CC3, 0x457CC3 + 10>([](injector::reg_pack& regs)
		{
			if (!CPad::GetPad(0)->GetTarget() && displayHelp) {
				char* message = TheText.Get((char*)"PU_CF1");

				// Original:	Press ~k~~PED_ANSWER_PHONE~ to pick up this weapon. It will replace any weapon you have of the same type.
				// We want:		Press ~k~~PED_ANSWER_PHONE~ to pick up this weapon.
				const char* ptr = strchr(message, '.');
				if (ptr) {
					int index = ptr - message;
					message[index + 1] = 0x0;
				}

				CHud::SetHelpMessage(message, 0, 0, 0);
				displayHelp = false;
			}
			// should always pickup is using vehicle or 2 players
			if (CPad::GetPad(0)->NewState.LeftShoulder1 || FindPlayerPed(0)->m_pVehicle || CGameLogic::IsCoopGameGoingOn())
			{
				// return valid to pick
				*(uintptr_t*)(regs.esp - 0x4) = 0x457BDE;
			}
			else
			{
				CPickups::PlayerOnWeaponPickup = 6; //original code
			}
		});

    }
} noAutoPickups;
