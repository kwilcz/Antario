#include <thread>
#include "Hooks.h"
#include "Utils\Utils.h"
#include "Features\Features.h"

Misc     g_Misc;
Hooks    g_Hooks;
Settings g_Settings;


void Hooks::Init()
{
    // Get window handle
    while (!(g_Hooks.hCSGOWindow = FindWindowA("Valve001", nullptr)))
    {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(50ms);
    }

    interfaces::Init();                         // Get interfaces
    g_pNetvars = std::make_unique<NetvarTree>();// Get netvars after getting interfaces as we use them

    Utils::Log("Hooking in progress...");


    // D3D Device pointer
    const uintptr_t d3dDevice = **reinterpret_cast<uintptr_t**>(Utils::FindSignature("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);

    if (g_Hooks.hCSGOWindow)        // Hook WNDProc to capture mouse / keyboard input
        g_Hooks.pOriginalWNDProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_Hooks.hCSGOWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Hooks::WndProc)));


    // VMTHooks
    g_Hooks.pD3DDevice9Hook = std::make_unique<VMTHook>(reinterpret_cast<void*>(d3dDevice));
    g_Hooks.pClientModeHook = std::make_unique<VMTHook>(g_pClientMode);
    g_Hooks.pSurfaceHook	= std::make_unique<VMTHook>(g_pSurface);

    // Hook the table functions
    g_Hooks.pD3DDevice9Hook->Hook(vtable_indexes::reset,      Hooks::Reset);
    g_Hooks.pD3DDevice9Hook->Hook(vtable_indexes::present,    Hooks::Present);
    g_Hooks.pClientModeHook->Hook(vtable_indexes::createMove, Hooks::CreateMove);
    g_Hooks.pSurfaceHook   ->Hook(vtable_indexes::lockCursor, Hooks::LockCursor);


    // Create event listener, no need for it now so it will remain commented.
    //const std::vector<const char*> vecEventNames = { "" };
    //g_Hooks.pEventListener = std::make_unique<EventListener>(vecEventNames);

    Utils::Log("Hooking completed!");
}


void Hooks::Restore()
{
	Utils::Log("Unhooking in progress...");
    {   // Unhook every function we hooked and restore original one
        g_Hooks.pD3DDevice9Hook->Unhook(vtable_indexes::reset);
        g_Hooks.pD3DDevice9Hook->Unhook(vtable_indexes::present);
        g_Hooks.pClientModeHook->Unhook(vtable_indexes::createMove);
        g_Hooks.pSurfaceHook->Unhook(vtable_indexes::lockCursor);
        SetWindowLongPtr(g_Hooks.hCSGOWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_Hooks.pOriginalWNDProc));

        g_pNetvars.reset();   /* Need to reset by-hand, global pointer so doesnt go out-of-scope */
    }
    Utils::Log("Unhooking succeded!");

    // Destroy fonts and all textures we created
    g_Render.InvalidateDeviceObjects();
    g_Fonts.DeleteDeviceObjects();
}


bool __fastcall Hooks::CreateMove(IClientMode* thisptr, void* edx, float sample_frametime, CUserCmd* pCmd)
{
    // Call original createmove before we start screwing with it
    static auto oCreateMove = g_Hooks.pClientModeHook->GetOriginal<CreateMove_t>(24);
    oCreateMove(thisptr, edx, sample_frametime, pCmd);

    if (!pCmd || !pCmd->command_number)
        return oCreateMove;

    // Get globals
    g::pCmd         = pCmd;
    g::pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());
    if (!g::pLocalEntity)
        return oCreateMove;


    g_Misc.OnCreateMove();
    // run shit outside enginepred

    engine_prediction::RunEnginePred();
    // run shit in enginepred
    engine_prediction::EndEnginePred();

    return false;
}


void __fastcall Hooks::LockCursor(ISurface* thisptr, void* edx)
{
    static auto oLockCursor = g_Hooks.pSurfaceHook->GetOriginal<LockCursor_t>(vtable_indexes::lockCursor);

    if (!g_Settings.bMenuOpened)
        return oLockCursor(thisptr, edx);

    g_pSurface->UnlockCursor();
}


HRESULT __stdcall Hooks::Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    static auto oReset = g_Hooks.pD3DDevice9Hook->GetOriginal<Reset_t>(16);

    if (g_Hooks.bInitializedDrawManager)
    {
        Utils::Log("Reseting draw manager.");
        g_Render.InvalidateDeviceObjects();
        HRESULT hr = oReset(pDevice, pPresentationParameters);
        g_Render.RestoreDeviceObjects(pDevice);
        Utils::Log("DrawManager reset succeded.");
        return hr;
    }

    return oReset(pDevice, pPresentationParameters);
}


HRESULT __stdcall Hooks::Present(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, 
                                 HWND hDestWindowOverride,  const RGNDATA* pDirtyRegion)
{
    IDirect3DStateBlock9* stateBlock = nullptr;
    pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &stateBlock);

    [pDevice]()
    {
        if (!g_Hooks.bInitializedDrawManager)
        {
            Utils::Log("Initializing Draw manager");
            g_Render.InitDeviceObjects(pDevice);
            g_Hooks.nMenu.Initialize();
            g_Hooks.bInitializedDrawManager = true;
            Utils::Log("Draw manager initialized");
        }
        else
        {
            g_Render.SetupRenderStates(); // Sets up proper render states for our state block

            static std::string szWatermark = "Antario";
            g_Render.String(8, 8, CD3DFONT_DROPSHADOW, Color(250, 150, 200, 180), g_Fonts.pFontTahoma8.get(), szWatermark.c_str());

            if (g_Settings.bMenuOpened)
            {
                g_Hooks.nMenu.Render();             // Render our menu
                g_Hooks.nMenu.mouseCursor->Render();// Render mouse cursor in the end so its not overlapped
            }

            // Put your draw calls here
            g_ESP.Render();
        }
    }();

    stateBlock->Apply();
    stateBlock->Release();

    static auto oPresent = g_Hooks.pD3DDevice9Hook->GetOriginal<Present_t>(17);
    return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}


LRESULT Hooks::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // for now as a lambda, to be transfered somewhere
    // Thanks uc/WasserEsser for pointing out my mistake!
    // Working when you HOLD th button, not when you press it.
    const auto getButtonHeld = [uMsg, wParam](bool& bButton, int vKey)
    {
		if (wParam != vKey) return;

        if (uMsg == WM_KEYDOWN)
            bButton = true;
        else if (uMsg == WM_KEYUP)
            bButton = false;
    };

	const auto getButtonToggle = [uMsg, wParam](bool& bButton, int vKey)
	{
		if (wParam != vKey) return;

		if (uMsg == WM_KEYUP)
			bButton = !bButton;
	};

	getButtonToggle(g_Settings.bMenuOpened, VK_INSERT);

    if (g_Hooks.bInitializedDrawManager)
    {
        // our wndproc capture fn
        if (g_Settings.bMenuOpened)
        {
            g_Hooks.nMenu.MsgProc(uMsg, wParam, lParam);
            return true;
        }
    }

    // Call original wndproc to make game use input again
    return CallWindowProcA(g_Hooks.pOriginalWNDProc, hWnd, uMsg, wParam, lParam);
}
