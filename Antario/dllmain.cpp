#include "Hooks.h"

using ulong_t = unsigned long;

void __stdcall on_attach() { // initialize stuff in here
    Hooks::Init();
}

void __stdcall on_detach() { // restore stuff in here
    Hooks::Restore();
}

bool __stdcall DllMain(HINSTANCE dll_instance, ulong_t reason_for_call, void *reserved) {
    switch (reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(dll_instance);
        CreateThread(0, 0, LPTHREAD_START_ROUTINE(on_attach), 0, 0, 0);

        return true;
        break;

    case DLL_PROCESS_DETACH:
        if ( reserved == nullptr ) {
            on_detach();
        }

        return true;
        break;

    default:
        return false;
        break;
    }
}
