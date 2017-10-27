#ifdef _WIN32
#pragma once
#include <Windows.h>
#endif
#include <memory>
#include "IClientMode.h"
#include "IBaseClientDll.h"


class Interfaces
{
public:
    void Init();
private:
    void GetIClientDll();
    void GetIClientMode();
};
extern Interfaces* g_pInterfaces;