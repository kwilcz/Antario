#include "EnginePrediction.h"
#include "..\Utils\GlobalVars.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\CGlobalVarsBase.h"

float flOldCurtime;
float flOldFrametime;


void EnginePrediction::RunEnginePred()
{
    static int flTickBase;
    static CUserCmd* pLastCmd;

    // Don't work on global variables.
    auto pLocal = g::pLocalEntity;
    auto pCmd   = g::pCmd;

    // fix tickbase if game didnt render previous tick
    if (pLastCmd)
    {
        if (pLastCmd->hasbeenpredicted)
            flTickBase = pLocal->GetTickBase();
        else
        {
            ++flTickBase;
            pLastCmd->hasbeenpredicted = true;
        }
    }

    // get random_seed as its 0 in clientmode->createmove
    auto getRandomSeed = [&pCmd]()
    {
        typedef std::uintptr_t(__cdecl* MD5_PseudoRandom_t)(std::uintptr_t);
        static auto MD5_PseudoRandom = (MD5_PseudoRandom_t)Utils::FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 70 6A 58");
        return MD5_PseudoRandom(pCmd->command_number) & 0x7FFFFFFF;
    };


    pLastCmd        = pCmd;
    flOldCurtime    = g_pGlobalVars->curtime;
    flOldFrametime  = g_pGlobalVars->frametime;

    g::uRandomSeed              = getRandomSeed();
    g_pGlobalVars->curtime      = flTickBase * g_pGlobalVars->intervalPerTick;
    g_pGlobalVars->frametime    = g_pGlobalVars->intervalPerTick;

    g_pMovement->StartTrackPredictionErrors(pLocal);

    CMoveData data;
    memset(&data, 0, sizeof(CMoveData));

    g_pPrediction->SetupMove(pLocal, pCmd, nullptr, &data);
    g_pMovement->ProcessMovement(pLocal, &data);
    g_pPrediction->FinishMove(pLocal, pCmd, &data);
}

void EnginePrediction::EndEnginePred()
{
    auto pLocal = g::pLocalEntity;
    g_pMovement->FinishTrackPredictionErrors(pLocal);

    g_pGlobalVars->curtime      = flOldCurtime;
    g_pGlobalVars->frametime    = flOldFrametime;
}
