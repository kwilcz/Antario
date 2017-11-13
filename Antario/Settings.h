#pragma once
#include "SDK\CEntity.h"

// Macro changing a variable name to a char*
#define GET_VAR_NAME(var) (#var)

struct Settings
{
    bool bBhopEnabled = true;
} settings;

