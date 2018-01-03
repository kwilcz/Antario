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

    // fix tickbase if game didnt render previous tick
    if (pLastCmd)
    {
        if (pLastCmd->hasbeenpredicted)
            flTickBase = g::pLocalEntity->GetTickBase();
        else
            ++flTickBase;
    }

    // get random_seed as its 0 in clientmode->createmove
    auto getRandomSeed = []()
    {
		using MD5_PseudoRandom = unsigned long(__cdecl*)(std::uintptr_t);
		static auto offset = Utils::FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 70 6A 58");
		static auto md5 = (MD5_PseudoRandom)(offset);
		return md5(g::pCmd->command_number) & 0x7FFFFFFF;
    };


    pLastCmd        = g::pCmd;
    flOldCurtime    = g_pGlobalVars->curtime;
    flOldFrametime  = g_pGlobalVars->frametime;

    g::uRandomSeed              = getRandomSeed();
    g_pGlobalVars->curtime      = flTickBase * g_pGlobalVars->intervalPerTick;
    g_pGlobalVars->frametime    = g_pGlobalVars->intervalPerTick;

    g_pMovement->StartTrackPredictionErrors(g::pLocalEntity);

    CMoveData data;
    memset(&data, 0, sizeof(CMoveData));

    g_pPrediction->SetupMove(g::pLocalEntity, g::pCmd, nullptr, &data);
    g_pMovement->ProcessMovement(g::pLocalEntity, &data);
    g_pPrediction->FinishMove(g::pLocalEntity, g::pCmd, &data);
}

void EnginePrediction::EndEnginePred()
{
    g_pMovement->FinishTrackPredictionErrors(g::pLocalEntity);

    g_pGlobalVars->curtime      = flOldCurtime;
    g_pGlobalVars->frametime    = flOldFrametime;
}
