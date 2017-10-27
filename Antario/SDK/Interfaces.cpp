#include "Interfaces.h"
#include "..\Utils\Utils.h"

Interfaces*     g_pInterfaces;
Utils*          g_pUtils;
// Initializing global interfaces

IBaseClientDLL* g_pClientDll    = nullptr;
IClientMode*    g_pClientMode   = nullptr;

template<typename T>
T* CaptureInterface(CreateInterfaceFn f, const char* szInterfaceVersion)
{
    return (T*)f(szInterfaceVersion, NULL);
}
void Interfaces::Init()
{
    this->GetIClientDll();
    this->GetIClientMode();
}

void Interfaces::GetIClientDll()
{
    CreateInterfaceFn pfnFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface");
    g_pClientDll = CaptureInterface<IBaseClientDLL>(pfnFactory, "VClient018");
}
void Interfaces::GetIClientMode()
{
    auto uAddress = *(DWORD*)(g_pUtils->FindSignature("client.dll", "8B 0D ? ? ? ? 8B 01 5D FF") + 2);
    g_pClientMode = *(IClientMode**)(uAddress);
}