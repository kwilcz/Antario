#pragma once
#include "..\Utils\GlobalVars.h"
#include "..\Settings.h"

class Misc
{
public:
    void OnCreateMove()
    {
        this->pCmd   = g::pCmd;
        this->pLocal = g::pLocalEntity;

        if (g_Settings.bBhopEnabled)
            this->DoBhop();

		if (g_Settings.bAirStrafeEnabled && !(pLocal->GetFlags() & FL_ONGROUND))
			DoAirStrafe();
        // sum future shit
    };
private:
    CUserCmd*     pCmd;
    C_BaseEntity* pLocal;

    void DoBhop() const
    {
        if (!(this->pLocal->GetFlags() & EntityFlags::FL_ONGROUND) && this->pLocal->GetMoveType() != MoveType_t::
            MOVETYPE_LADDER)
            if (this->pCmd->buttons & IN_JUMP)
                this->pCmd->buttons &= ~IN_JUMP;
    }

	void DoAirStrafe() const
    {
		if (pCmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK))
			return;

		const auto maxSpeed = pLocal->GetMaxSpeed();
		pCmd->forwardmove = min(850 * 3 / pLocal->GetVelocity().Length2D(), maxSpeed);

		pCmd->sidemove = pCmd->tick_count % 2 == 0 ? maxSpeed : -maxSpeed;

		if (pCmd->mousedx < 0)
			pCmd->sidemove = -maxSpeed;
		else if (pCmd->mousedy > 0)
			pCmd->sidemove = maxSpeed;
    }
};

extern Misc g_Misc;
