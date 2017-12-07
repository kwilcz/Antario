#pragma once
#include "SDK\CEntity.h"

// Macro changing a variable name to a char*
#define GET_VAR_NAME(var) (#var)

struct Settings
{
    bool bMenuOpened    = false;
    bool bBhopEnabled   = true;
    Color colCursor     = Color(0, 150, 255, 100);
} g_Settings;

