#include "Interfaces.h"
#include "Utils.h"

#include "..\SDK\IClientMode.h"
#include "..\SDK\IBaseClientDll.h"
#include "..\SDK\IClientEntityList.h"
#include "..\SDK\IVEngineClient.h"

// Initializing global variables with no cpp file matching the header
Utils g_pUtils;
// Initializing global interfaces

IBaseClientDLL*     g_pClientDll = nullptr;
IClientMode*        g_pClientMode = nullptr;
IClientEntityList*  g_pEntityList   = nullptr;
IVEngineClient*     g_pEngineClient = nullptr;


namespace Interfaces
{
    template<typename T>
    T* CaptureInterface(CreateInterfaceFn pfnFactory, const char* szInterfaceVersion)
    {
        return (T*)pfnFactory(szInterfaceVersion, NULL);
    }
    
    void Init()
    {
        GetIClientDll();
        GetIClientMode();
        GetIClientEntityList();
        GetIVEngineClient();
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
    void GetIClientEntityList()
    {
        CreateInterfaceFn pfnFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface");
        g_pEntityList = CaptureInterface<IClientEntityList>(pfnFactory, "VClientEntityList003");
    }
    void GetIVEngineClient()
    {
        CreateInterfaceFn pfnFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface");
        g_pEngineClient = CaptureInterface<IVEngineClient>(pfnFactory, "VClientEntityList003");
    }
}
