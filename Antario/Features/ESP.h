#pragma once
#include <deque>
#include "..\Utils\DrawManager.h"
#include "..\Utils\GlobalVars.h"
#include "..\Settings.h"


class ESP
{
public:
    void Render();
private:
    void RenderName(C_BaseEntity* pEnt, int iterator);
};
extern ESP g_ESP;
