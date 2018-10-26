#include "Interfaces.h"
#include "Utils.h"

#include "..\SDK\IClientMode.h"
#include "..\SDK\IBaseClientDll.h"
#include "..\SDK\IClientEntityList.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\IGameEvent.h"
#include "..\SDK\ISurface.h"

// Initializing global interfaces

IBaseClientDLL*     g_pClientDll = nullptr;
IClientMode*        g_pClientMode = nullptr;
IClientEntityList*  g_pEntityList = nullptr;
IVEngineClient*     g_pEngine = nullptr;
CPrediction*        g_pPrediction = nullptr;
IGameMovement*      g_pMovement = nullptr;
IMoveHelper*        g_pMoveHelper = nullptr;
CGlobalVarsBase*    g_pGlobalVars = nullptr;
IGameEventManager2* g_pEventManager = nullptr;
ISurface*           g_pSurface = nullptr;

#pragma pack(push, 1)
struct GameInterface
{
	void*(*func)();
	char* name;
	GameInterface* next;
};
#pragma pack(pop)

static GameInterface* GetInterfaceListFromFunction(CreateInterfaceFn func)
{
	return *(GameInterface**)(*(std::uintptr_t*)(((std::uintptr_t)func) - 0x6A));
}

static bool StartsWith(char const* str, char const* target)
{
	for (; *str && *target; ++str, ++target)
	{
		if (*str != *target)
		{
			return false;
		}
	}

	return true;
}

namespace interfaces
{
	template<typename T>
	T* CaptureInterface(const char* szModuleName, const char* szInterfaceVersion)
	{
		HMODULE moduleHandle = GetModuleHandleA(szModuleName);
		if (moduleHandle)   /* In case of not finding module handle, throw an error. */
		{
			CreateInterfaceFn pfnFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(moduleHandle, "CreateInterface"));
			if (pfnFactory)
			{
				auto i = GetInterfaceListFromFunction(pfnFactory);
				
				GameInterface* shortest = nullptr;
				std::size_t shortestLength = SIZE_MAX;

				do
				{
					if (StartsWith(i->name, szInterfaceVersion))
					{
						auto length = std::strlen(i->name);
						if (length < shortestLength)
						{
							shortestLength = length;
							shortest = i;
						}
					}
				} while (i = i->next);

				if (shortest)
				{
					return static_cast<T*>(shortest->func());
				}
			}
		}

		Utils::Log(std::string("Error getting interface ") + szInterfaceVersion);
		return nullptr;
	}


	void Init()
	{
		g_pClientDll = CaptureInterface<IBaseClientDLL>("client_panorama.dll", "VClient");					// Get IBaseClientDLL
		g_pClientMode = **reinterpret_cast<IClientMode***>    ((*reinterpret_cast<std::uintptr_t**>(g_pClientDll))[10] + 0x5u);  // Get IClientMode
		g_pGlobalVars = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<std::uintptr_t**>(g_pClientDll))[0] + 0x1Bu); // Get CGlobalVarsBase
		g_pEntityList = CaptureInterface<IClientEntityList>("client_panorama.dll", "VClientEntityList");    // Get IClientEntityList
		g_pEngine = CaptureInterface<IVEngineClient>("engine.dll", "VEngineClient");					  // Get IVEngineClient
		g_pPrediction = CaptureInterface<CPrediction>("client_panorama.dll", "VClientPrediction");          // Get CPrediction
		g_pMovement = CaptureInterface<IGameMovement>("client_panorama.dll", "GameMovement");             // Get IGameMovement
		g_pMoveHelper = **reinterpret_cast<IMoveHelper***>((Utils::FindSignature("client_panorama.dll", "8B 0D ? ? ? ? 8B 46 08 68") + 0x2));  // Get IMoveHelper
		g_pEventManager = CaptureInterface<IGameEventManager2>("engine.dll", "GAMEEVENTSMANAGER002");		  // Get IGameEventManager2
		g_pSurface = CaptureInterface<ISurface>("vguimatsurface.dll", "VGUI_Surface");					  // Get ISurface
	}
}
