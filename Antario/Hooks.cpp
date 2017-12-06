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
    g_Hooks.pD3DDevice9Hook = std::make_unique<VMTHook>(reinterpret_cast<void*>(d3dDevice));
    g_Hooks.pClientModeHook = std::make_unique<VMTHook>(g_pClientMode);

    // Hook the table functions
    g_Hooks.pD3DDevice9Hook->Hook(16, Hooks::Reset);
    g_Hooks.pD3DDevice9Hook->Hook(17, Hooks::Present);
    g_Hooks.pClientModeHook->Hook(24, Hooks::CreateMove); 


   
#ifdef _DEBUG       // Create console only in debug mode
    AllocConsole();                                             // Alloc memory and create console    
    g_Utils.SetConsoleHandle(GetStdHandle(STD_OUTPUT_HANDLE));  // Get and save console handle to future use with WriteToConsole etc.    
    g_Utils.LogToConsole(L"Initialization Succeded \n");        // Log info to console
#endif
}



void Hooks::Restore()
{
    // Unhook every function we hooked and restore original one
    g_Hooks.pD3DDevice9Hook->Unhook(16);
    g_Hooks.pD3DDevice9Hook->Unhook(17);
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

    // Local player, get only once, for now in cm only. Will make it global later on
    auto pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

    g_Misc.OnCreateMove(pCmd);
    // run shit outside enginepred

    EnginePrediction::RunEnginePred(pLocalEntity, pCmd);
    // run shit in enginepred
    EnginePrediction::EndEnginePred(pLocalEntity);

    return false;
}




HRESULT __stdcall Hooks::Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    static auto oReset = g_Hooks.pD3DDevice9Hook->GetOriginal<Reset_t>(16);

    if (g_Hooks.bInitializedDrawManager)
    {
        g_Render.InvalidateDeviceObjects();
        HRESULT hr = oReset(pDevice, pPresentationParameters);
        g_Render.RestoreDeviceObjects(pDevice);

        return hr;
    }

    return oReset(pDevice, pPresentationParameters);

}




HRESULT __stdcall Hooks::Present(IDirect3DDevice9 * pDevice, const RECT * pSourceRect, const RECT * pDestRect, HWND hDestWindowOverride, const RGNDATA * pDirtyRegion)
{
    if (!g_Hooks.bInitializedDrawManager)
    {
        g_Render.InitDeviceObjects(pDevice);
        g_Hooks.bInitializedDrawManager = true;
    }
    else
    {
        // watermark to distinguish if we injected (for now)
        std::string szWatermark = "Antario, build 06.12.2017";
        g_Render.DrawString(8, 8, D3DFONT_SHADOW, Color(250, 150, 200, 180), g_Fonts.pFontTahoma8.get(), szWatermark.c_str());
        // draw here
    }
    static auto oPresent = g_Hooks.pD3DDevice9Hook->GetOriginal<Present_t>(17);
    return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}
