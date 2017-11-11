#ifdef _WIN32
#pragma once
#include <Windows.h>
#endif
#include <memory>


namespace Interfaces
{
    // Used to initialize all the interfaces at one time
    void Init();
};