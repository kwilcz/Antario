#include "Interfaces.h"
#include "Utils.h"

#include "..\SDK\IClientMode.h"
#include "..\SDK\IBaseClientDll.h"
#include "..\SDK\IClientEntityList.h"
#include "..\SDK\IVEngineClient.h"

// Initializing global variables with no cpp file matching the header

Utils g_pUtils;

// Initializing global interfaces

IBaseClientDLL*     g_pClientDll    = nullptr;
IClientMode*        g_pClientMode   = nullptr;
IClientEntityList*  g_pEntityList   = nullptr;
IVEngineClient*     g_pEngine       = nullptr;


namespace Interfaces
{
    template<typename T>
    T* CaptureInterface(const char* szModuleName, const char* szInterfaceVersion)
    {
        CreateInterfaceFn pfnFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(szModuleName), "CreateInterface");        
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
        g_pClientDll = CaptureInterface<IBaseClientDLL>("client.dll", "VClient018");
    }
    void GetIClientMode()
    {
        g_pClientMode = **(IClientMode***)((*(uintptr_t**)g_pClientDll)[10] + 0x5);
    }
    void GetIClientEntityList()
    {
        g_pEntityList = CaptureInterface<IClientEntityList>("client.dll", "VClientEntityList003");
    }
    void GetIVEngineClient()
    {
        g_pEngine = CaptureInterface<IVEngineClient>("engine.dll", "VEngineClient014");
    }
}
