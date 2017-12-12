#include <thread>
#include "Hooks.h"
#include "Utils\Utils.h"
#include "Settings.h"
#include "Utils\DrawManager.h"
DWORD WINAPI OnDllAttach(PVOID base)
{
#ifdef _DEBUG       // Create console only in debug mode
    AllocConsole();                                 // Alloc memory and create console    
    freopen_s((FILE**)stdin,  "CONIN$", "r",  stdin);   // ----------------------------------------------
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);  //  Make iostream library use our console handle
    freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);  // ----------------------------------------------
    SetConsoleTitleA(" Antario - Debug console");   // Set console name to a custom one
#endif
    g_Utils.Log("Console Allocated\n");
    Hooks::Init();

    while (g_Settings.bCheatActive)
    {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
    FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
}

VOID WINAPI OnDllDetach()
{
#ifdef _DEBUG
    FreeConsole();  // Free allocated memory and remove console
#endif
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) 
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)OnDllAttach, NULL, NULL, NULL);
    }
    else if (dwReason == DLL_PROCESS_DETACH) 
    {
        if (!lpReserved)
            Hooks::Restore();

        OnDllDetach();
    }
    return TRUE;
}

