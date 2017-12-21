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
    void Render()
    {
        this->DrawRecoilCrosshair();
    };
private:
    CUserCmd* pCmd;
    C_BaseEntity* pLocal;

    void DoBhop()
    {
        if (!(this->pLocal->GetFlags() & EntityFlags::FL_ONGROUND) && this->pLocal->GetMoveType() != MoveType_t::MOVETYPE_LADDER)
            if (this->pCmd->buttons & IN_JUMP)
                this->pCmd->buttons &= ~IN_JUMP;
    }
    void DrawRecoilCrosshair()
    {

    }
};
extern Misc g_Misc;
