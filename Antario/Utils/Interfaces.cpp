#include "Interfaces.h"
#include "Utils.h"

Utils*          g_pUtils;
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
        auto uAddress = *(DWORD*)(g_pUtils->FindSignature("client.dll", "8B 0D ? ? ? ? 8B 01 5D FF") + 2);
        g_pClientMode = *(IClientMode**)(uAddress);
    }
}