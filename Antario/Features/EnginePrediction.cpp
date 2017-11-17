#include "EnginePrediction.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\CGlobalVarsBase.h"

float flOldCurtime;
float flOldFrametime;

void EnginePrediction::RunEnginePred(C_BaseEntity* pLocal, CUserCmd* pCmd)
{
    if (!pLocal || !pCmd)
        return;

    static CUserCmd* pLastCmd;
    static unsigned int flTickBase;

    // fix tickbase if game didnt render prevoious tick, not tested - hasbeenpredicted could be always false. If yes, comment it.
    if (pLastCmd)   
        if (pLastCmd->hasbeenpredicted)
            flTickBase = pLocal->GetTickBase();
        else
            ++flTickBase;

    pLastCmd        = pCmd;
    flOldCurtime    = g_pGlobalVars->curtime;
    flOldFrametime  = g_pGlobalVars->frametime;

    g_pGlobalVars->curtime      = pLocal->GetTickBase() * g_pGlobalVars->intervalPerTick;
    g_pGlobalVars->frametime    = g_pGlobalVars->intervalPerTick;

    g_pMovement->StartTrackPredictionErrors(pLocal);

    CMoveData data;
    memset(&data, 0, sizeof(CMoveData));

    g_pPrediction->SetupMove(pLocal, pCmd, nullptr, &data);
    g_pMovement->ProcessMovement(pLocal, &data);
    g_pPrediction->FinishMove(pLocal, pCmd, &data);
}

void EnginePrediction::EndEnginePred(C_BaseEntity* pLocal)
{
    g_pMovement->FinishTrackPredictionErrors(pLocal);

    g_pGlobalVars->curtime      = flOldCurtime;
    g_pGlobalVars->frametime    = flOldFrametime;
}
