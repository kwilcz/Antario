#include "Interfaces.h"
#include "Utils.h"

#include "..\SDK\IClientMode.h"
#include "..\SDK\IBaseClientDll.h"
#include "..\SDK\IClientEntityList.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\CPrediction.h"

// Initializing global variables with no cpp file matching the header

Utils g_Utils;

// Initializing global interfaces

IBaseClientDLL*     g_pClientDll    = nullptr;
IClientMode*        g_pClientMode   = nullptr;
IClientEntityList*  g_pEntityList   = nullptr;
IVEngineClient*     g_pEngine       = nullptr;
CPrediction*        g_pPrediction   = nullptr;
IGameMovement*      g_pMovement     = nullptr;
CGlobalVarsBase*    g_pGlobalVars   = nullptr;


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
        g_pClientDll    = CaptureInterface<IBaseClientDLL>("client.dll", "VClient018");                 // Get IBaseClientDLL
        g_pClientMode   = **reinterpret_cast<IClientMode***>    ((*reinterpret_cast<uintptr_t**>(g_pClientDll))[10] + 0x5);  // Get IClientMode
        g_pGlobalVars   = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<uintptr_t**>(g_pClientDll))[0]  + 0x1B); // Get CGlobalVarsBase
        g_pEntityList   = CaptureInterface<IClientEntityList>("client.dll", "VClientEntityList003");    // Get IClientEntityList
        g_pEngine       = CaptureInterface<IVEngineClient>("engine.dll", "VEngineClient014");           // Get IVEngineClient
        g_pPrediction   = CaptureInterface<CPrediction>("client.dll", "VClientPrediction001");          // Get CPrediction
        g_pMovement     = CaptureInterface<IGameMovement>("client.dll", "GameMovement001");             // Get IGameMovement
    }
}
