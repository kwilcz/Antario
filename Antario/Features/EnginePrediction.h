#pragma once
#include "..\SDK\CInput.h"
#include "..\SDK\CEntity.h"

namespace EnginePrediction
{
    void RunEnginePred(C_BaseEntity* pLocal, CUserCmd* pCmd);
    void EndEnginePred(C_BaseEntity* pLocal);

    // random seed we can use on nospread servers
    extern int* m_nRandomSeed;
}