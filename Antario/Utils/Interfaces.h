#ifdef _WIN32
#pragma once
#include <Windows.h>
#endif
#include <memory>
#include "..\SDK\IClientMode.h"
#include "..\SDK\IBaseClientDll.h"


namespace Interfaces
{
    void Init();
    void GetIClientDll();
    void GetIClientMode();
};