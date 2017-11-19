#include "EnginePrediction.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\CGlobalVarsBase.h"

float flOldCurtime;
float flOldFrametime;

int EnginePrediction::m_iRandomSeed = 0;

void EnginePrediction::RunEnginePred(C_BaseEntity* pLocal, CUserCmd* pCmd)
{
    if (!pLocal || !pCmd)
        return;

    static CUserCmd* pLastCmd;
    static unsigned int flTickBase;

    // fix tickbase if game didnt render prevoious tick
    if (pLastCmd)   
        if (pLastCmd->hasbeenpredicted)
            flTickBase = pLocal->GetTickBase();
        else
            ++flTickBase;

    // get random_seed as its 0 in clientmode->createmove
    typedef UINT(__cdecl* MD5_PseudoRandom_t)(UINT);
    static auto MD5_PseudoRandom = (MD5_PseudoRandom_t)g_Utils.FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 70 6A 58");
    m_iRandomSeed = MD5_PseudoRandom(pCmd->command_number) & 0x7fffffff;


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
