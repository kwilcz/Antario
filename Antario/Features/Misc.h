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
};

extern Misc g_Misc;
