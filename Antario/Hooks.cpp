#include "Hooks.h"
#include "Utils\Utils.h"
#include "Features\Misc.h"
#include "Features\EnginePrediction.h"
#include "SDK\IClientMode.h"

Misc    g_Misc;
Hooks   g_Hooks;

void Hooks::Init()
{
    Interfaces::Init();                         // Get interfaces
    g_pNetvars = std::make_unique<NetvarTree>();// Get netvars after getting interfaces as we use them

    // D3D Device pointer
    uintptr_t d3dDevice = **(uintptr_t**)(g_Utils.FindSignature("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);

    // VMTHooks
    //g_pHooks->pD3DDevice9Hook = std::make_unique<VMTHook>(reinterpret_cast<void*>(d3dDevice));
    g_Hooks.pClientModeHook = std::make_unique<VMTHook>(g_pClientMode);
    g_Hooks.pClientModeHook->Hook(24, (void*)Hooks::CreateMove); 


   
#ifdef _DEBUG       // Example on how to use external WIN console.
    AllocConsole();                                             // Alloc memory and create console    
    g_Utils.SetConsoleHandle(GetStdHandle(STD_OUTPUT_HANDLE));  // Get and save console handle to future use with WriteToConsole etc.    
    g_Utils.LogToConsole(L"Initialization Succeded \n");        // Log info to console
#endif
}



void Hooks::Restore()
{
    // Unhook every function we hooked and restore original one
    g_Hooks.pClientModeHook->Unhook(24);

#ifdef _DEBUG
    FreeConsole();  // Free allocated memory and remove console
#endif
}



bool __fastcall Hooks::CreateMove(IClientMode* thisptr, void* edx, float sample_frametime, CUserCmd* pCmd)
{
    // Call original createmove before we start screwing with it
    static auto oCreateMove = g_Hooks.pClientModeHook->GetOriginal<CreateMove_t>(24);
    oCreateMove(thisptr, edx, sample_frametime, pCmd);

    if (!pCmd->command_number)
        return oCreateMove;
    
    // todo update the engine with our current angles ; generate pCmd->random_seed ; get send_packet off the stack

    // Some quick debug log, uncomment to check if everything works
//#ifdef _DEBUG
//    std::wstring str = std::to_wstring(g_pEngine->GetLocalPlayer());
//    g_Utils.LogToConsole(str);
//#endif // _DEBUG

    // Local player, get only once -> OPTIMALIZATION :)
    auto pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

    g_Misc.OnCreateMove(pCmd);
    // run shit outside enginepred
    
    EnginePrediction::RunEnginePred(pLocalEntity, pCmd);
    // run shit in enginepred
    EnginePrediction::EndEnginePred(pLocalEntity);
    
    // todo clamp movement here with std::clamp and add a vector clamp fn and call it here
    
    return false; // return false here so that the engine doesn't update our view_angles without any modified angles
}
