#include "Interfaces.h"
#include "Utils.h"

Utils           g_pUtils;
// Initializing global interfaces

IBaseClientDLL* g_pClientDll    = nullptr;
IClientMode*    g_pClientMode   = nullptr;


namespace Interfaces
{
    template<typename T>
    T* CaptureInterface(CreateInterfaceFn f, const char* szInterfaceVersion)
    {
        return (T*)f(szInterfaceVersion, NULL);
    }
    
    void Init()
    {
        GetIClientDll();
        GetIClientMode();
    }



    void GetIClientDll()
    {
        CreateInterfaceFn pfnFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface");
        g_pClientDll = CaptureInterface<IBaseClientDLL>(pfnFactory, "VClient018");
    }
    void GetIClientMode()
    {
        g_pClientMode = **(IClientMode***)((*(uintptr_t**)g_pClientDll)[10] + 0x5);
    }
}
