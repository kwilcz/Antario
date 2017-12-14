#include <thread>
#include "Hooks.h"
#include "Utils\Utils.h"
#include "Features\Misc.h"
#include "Features\EnginePrediction.h"
#include "SDK\IClientMode.h"

Misc     g_Misc;
Hooks    g_Hooks;
Settings g_Settings;


void Hooks::Init()
{
    // Get window handle
    while (!(g_Hooks.hCSGOWindow = FindWindowA("Valve001", NULL)))
    {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(50ms);
    }
    
    Interfaces::Init();                         // Get interfaces
    g_pNetvars = std::make_unique<NetvarTree>();// Get netvars after getting interfaces as we use them

    g_Utils.Log("Hooking in progress...\n");
    // D3D Device pointer
    uintptr_t d3dDevice = **(uintptr_t**)(g_Utils.FindSignature("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);

    if (g_Hooks.hCSGOWindow)        // Hook WNDProc to capture mouse / keyboard input
        g_Hooks.pOriginalWNDProc = (WNDPROC)SetWindowLongPtr(g_Hooks.hCSGOWindow, GWLP_WNDPROC, (LONG_PTR)g_Hooks.WndProc);

    // VMTHooks
    g_Hooks.pD3DDevice9Hook = std::make_unique<VMTHook>(reinterpret_cast<void*>(d3dDevice));
    g_Hooks.pClientModeHook = std::make_unique<VMTHook>(g_pClientMode);

    // Hook the table functions
    g_Hooks.pD3DDevice9Hook->Hook(16, Hooks::Reset);
    g_Hooks.pD3DDevice9Hook->Hook(17, Hooks::Present);
    g_Hooks.pClientModeHook->Hook(24, Hooks::CreateMove); 


    // Create event listener, no need for it now so it will remain commented.
    //g_Hooks.eventListener = new EventListener({ "" });

    g_Utils.Log("Hooking completed.\n");
}



void Hooks::Restore()
{
    g_Utils.Log("Unhooking in progress.\n");
    // Unhook every function we hooked and restore original one
    g_Hooks.pD3DDevice9Hook->Unhook(16);
    g_Hooks.pD3DDevice9Hook->Unhook(17);
    g_Hooks.pClientModeHook->Unhook(24);
    SetWindowLongPtr(g_Hooks.hCSGOWindow, GWLP_WNDPROC, (LONG_PTR)g_Hooks.pOriginalWNDProc);
    g_Utils.Log("Unhooking succeded!\n");

    // Destroy fonts and all textures we created
    g_Render.InvalidateDeviceObjects();
    g_Fonts.DeleteDeviceObjects();
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
        g_Utils.Log("Reseting draw manager.\n");
        g_Render.InvalidateDeviceObjects();
        HRESULT hr = oReset(pDevice, pPresentationParameters);
        g_Render.RestoreDeviceObjects(pDevice);
        g_Utils.Log("DrawManager reset succeded.\n");
        return hr;
    }

    return oReset(pDevice, pPresentationParameters);

}




HRESULT __stdcall Hooks::Present(IDirect3DDevice9 * pDevice, const RECT * pSourceRect, const RECT * pDestRect, HWND hDestWindowOverride, const RGNDATA * pDirtyRegion)
{
    if (!g_Hooks.bInitializedDrawManager)
    {
        g_Utils.Log("Initializing Draw manager\n");
        g_Render.InitDeviceObjects(pDevice);
        g_Hooks.nMenu.Initialize();
        g_Hooks.bInitializedDrawManager = true;
        g_Utils.Log("Draw manager initialized\n");
    }
    else
    {   
        static bool bIsHeld = false;
        if (g_Settings.bMenuOpened && !bIsHeld)
        {
            g_pEngine->ExecuteClientCmd("cl_mouseenable 0");
            bIsHeld = true;
        }
        else
        if (!g_Settings.bMenuOpened && bIsHeld)
        {
            g_pEngine->ExecuteClientCmd("cl_mouseenable 1");
            bIsHeld = false;
        }

        // watermark to distinguish if we injected (for now)
        std::string szWatermark = "Antario, build 09.12.2017";
        g_Render.String(8, 8, D3DFONT_DROPSHADOW, Color(250, 150, 200, 180), g_Fonts.pFontTahoma8.get(), szWatermark.c_str());

        if (g_Settings.bMenuOpened)
        {
            g_Hooks.nMenu.UpdateData();
            g_Hooks.nMenu.Render();     // Render our menu                                        
            g_Hooks.nMenu.mouseCursor->Render();// Render mouse cursor in the end so its not overlapped
        }

        // Put your draw calls here
    }
    static auto oPresent = g_Hooks.pD3DDevice9Hook->GetOriginal<Present_t>(17);
    return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}




LRESULT Hooks::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // for now as a lambda, to be transfered somewhere
    // Thanks uc/WasserEsser for pointing out my mistake!
    auto GetButtonHeld = [uMsg, wParam](bool& bButton, int vKey)
    {
        if (uMsg == WM_KEYDOWN && wParam == vKey)
            bButton = true;
        else
            if (uMsg == WM_KEYUP && wParam == vKey)
                bButton = false;
    };

    // Working when you HOLD the insert button, not when you press it.
     GetButtonHeld(g_Settings.bMenuOpened, VK_INSERT);

    if (g_Hooks.bInitializedDrawManager)
    {        
        // our wndproc capture fn
        g_Hooks.nMenu.RunThink(uMsg, lParam);

        if (g_Settings.bMenuOpened)
            return true;    // Disable game wndproc usage when we use the menu
    }

    // Call original wndproc to make game use input again
    return CallWindowProcA(g_Hooks.pOriginalWNDProc, hWnd, uMsg, wParam, lParam);
}
