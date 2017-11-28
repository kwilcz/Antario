#include "EnginePrediction.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\CGlobalVarsBase.h"

float flOldCurtime;
float flOldFrametime;

int* EnginePrediction::m_nRandomSeed = nullptr;

void EnginePrediction::RunEnginePred(C_BaseEntity* pLocal, CUserCmd* pCmd)
{
    static CUserCmd* pLastCmd;
    static int flTickBase;

    // fix tickbase if game didnt render previous tick
    if (pLastCmd)   
        if (pLastCmd->hasbeenpredicted)
            flTickBase = pLocal->GetTickBase();
        else
            ++flTickBase;

    // get random_seed as its 0 in clientmode->createmove
    typedef unsigned int(__cdecl* MD5_PseudoRandom_t)(unsigned int);
    static auto MD5_PseudoRandom = (MD5_PseudoRandom_t)g_Utils.FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 70 6A 58");
    *m_nRandomSeed = MD5_PseudoRandom(pCmd->command_number) & 0x7FFFFFFF;


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
