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
    C_BaseCombatWeapon* GetActiveWeapon()
    {
        static int m_hActiveWeapon = g_pNetvars->GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
        auto weaponData = GetValue<CBaseHandle>(m_hActiveWeapon);
        return reinterpret_cast<C_BaseCombatWeapon*>(g_pEntityList->GetClientEntityFromHandle(weaponData));
    }
    int GetTeam()
    {
        static int m_iTeamNum = g_pNetvars->GetOffset("DT_BaseEntity", "m_iTeamNum");
        return GetValue<int>(m_iTeamNum);
    }
    int& GetFlags()
    {
        static int m_fFlags = g_pNetvars->GetOffset("DT_BasePlayer", "m_fFlags");
        return *(int*)((std::uintptr_t)this + m_fFlags);    // template will not work here
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