#pragma once
#include "IClientUnknown.h"
#include "IClientEntityList.h"
#include "..\Utils\NetvarManager.h"

// class predefinition
class C_BaseCombatWeapon;

class C_BaseEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable
{
private:
    template <class T>
    T GetField(int offset)
    {
        return (T*)((std::uintptr_t)this + offset);
    }
    // To get value from the pointer itself
    template <class T>
    T GetValue(int offset)
    {
        return *(T*)((std::uintptr_t)this + offset);
    }

public:
    C_BaseCombatWeapon* GetActiveWeapon()
    {
        static int m_hActiveWeapon = g_pNetvars->GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
        CBaseHandle weaponData = GetValue<CBaseHandle>(m_hActiveWeapon);
        return (C_BaseCombatWeapon*)g_pEntityList->GetClientEntityFromHandle(weaponData);
    }
    int GetTeam()
    {
        static int m_iTeamNum = g_pNetvars->GetOffset("DT_BaseEntity", "m_iTeamNum");
        return GetValue<int>(m_iTeamNum);
    }

    /// in progress
};

class C_BaseCombatWeapon : public C_BaseEntity
{
private:
    template <class T>
    T GetPointer(int offset)
    {
        return (T*)((std::uintptr_t)this + offset);
    }
    // To get value from the pointer itself
    template <class T>
    T GetValue(int offset)
    {
        return *(T*)((std::uintptr_t)this + offset);
    }
public:
    /// in progress
};