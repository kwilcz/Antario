#pragma once
#include "SDK\CEntity.h"

// Macro changing a variable name to a char*
#define GET_VAR_NAME(var) (#var)

struct Settings
{
    bool bCheatActive   = true;
    bool bMenuOpened    = false;
    bool bBhopEnabled   = true;
    bool bShowNames     = true;
    Color colCursor     = Color(0, 150, 255, 100);
};
extern Settings g_Settings;

