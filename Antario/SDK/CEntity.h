#pragma once
#include "Definitions.h"
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
    int GetTeam() //DT_BaseAttributableItem"), charenc("m_iItemDefinitionIndex
    {
        static int m_iTeamNum = g_pNetvars->GetOffset("DT_BaseEntity", "m_iTeamNum");
        return GetValue<int>(m_iTeamNum);
    }
    EntityFlags GetFlags()
    {
        static int m_fFlags = g_pNetvars->GetOffset("DT_BasePlayer", "m_fFlags");
        return GetValue<EntityFlags>(m_fFlags);
    }
    MoveType_t GetMoveType()
    {
        static int m_Movetype = g_pNetvars->GetOffset("DT_BaseEntity", "m_nRenderMode") + 1;
        return GetValue<MoveType_t>(m_Movetype);
    }
    bool GetLifeState()
    {
        static int m_lifeState = g_pNetvars->GetOffset("DT_BasePlayer", "m_lifeState");
        return GetValue<bool>(m_lifeState);
    }
    int GetHealth()
    {
        static int m_iHealth = g_pNetvars->GetOffset("DT_BasePlayer", "m_iHealth");
        return GetValue<int>(m_iHealth);
    }
    bool IsAlive()
    {
        return this->GetHealth() > 0 && this->GetLifeState() == 0;
    }
    bool IsImmune()
    {
        static int m_bGunGameImmunity = g_pNetvars->GetOffset("DT_CSPlayer", "m_bGunGameImmunity");
        return GetValue<bool>(m_bGunGameImmunity);
    }
    int GetTickBase()
    {
        static int m_nTickBase = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_nTickBase");
        return GetValue<int>(m_nTickBase);
    }
    Vector GetOrigin()
    {
        static int m_vecOrigin = g_pNetvars->GetOffset("DT_BaseEntity", "m_vecOrigin");
        return GetValue<Vector>(m_vecOrigin);
    }	
    Vector GetViewOffset()
    {
        static int m_vecViewOffset = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
        return GetValue<Vector>(m_vecViewOffset);
    }
    Vector GetEyePosition()
    {
        return this->GetOrigin() + this->GetViewOffset();
    }
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
	ItemDefinitionIndex GetItemDefinitionIndex()
	{
		static int m_iItemDefinitionIndex = g_pNetvars->GetOffset("DT_BaseAttributableItem", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
		return GetValue<ItemDefinitionIndex>(m_iItemDefinitionIndex);
	}
	float GetNextPrimaryAttack()
	{
		static int m_flNextPrimaryAttack = g_pNetvars->GetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
		return GetValue<float>(m_flNextPrimaryAttack);
	}
	int GetAmmo()
	{
		static int m_iClip1 = g_pNetvars->GetOffset("DT_BaseCombatWeapon", "m_iClip1");
		return GetValue<int>(m_iClip1);
	}
	std::string GetName()
	{
		switch (this->GetItemDefinitionIndex())
		{ //xD
		case ItemDefinitionIndex::WEAPON_AK47: return			"ak47";
		case ItemDefinitionIndex::WEAPON_AUG: return			"aug";
		case ItemDefinitionIndex::WEAPON_AWP: return			"awp";
		case ItemDefinitionIndex::WEAPON_BIZON: return			"bizon";
		case ItemDefinitionIndex::WEAPON_CZ75A: return			"cz75";
		case ItemDefinitionIndex::WEAPON_DEAGLE: return			"deagle";
		case ItemDefinitionIndex::WEAPON_ELITE: return			"elite";
		case ItemDefinitionIndex::WEAPON_FIVESEVEN: return		"fiveseven";
		case ItemDefinitionIndex::WEAPON_GLOCK: return			"glock18";
		case ItemDefinitionIndex::WEAPON_FAMAS: return			"famas";
		case ItemDefinitionIndex::WEAPON_G3SG1: return			"g3sg1";
		case ItemDefinitionIndex::WEAPON_GALILAR: return		"galilar";
		case ItemDefinitionIndex::WEAPON_HKP2000: return		"hkp2000";
		case ItemDefinitionIndex::WEAPON_MAG7: return			"mag7";
		case ItemDefinitionIndex::WEAPON_M249: return			"m249";
		case ItemDefinitionIndex::WEAPON_M4A1: return			"m4a4";
		case ItemDefinitionIndex::WEAPON_M4A1_SILENCER: return  "m4a1s";
		case ItemDefinitionIndex::WEAPON_MAC10: return			"mac10";
		case ItemDefinitionIndex::WEAPON_MP7: return			"mp7";
		case ItemDefinitionIndex::WEAPON_MP9: return			"mp9";
		case ItemDefinitionIndex::WEAPON_NOVA: return			"nova";
		case ItemDefinitionIndex::WEAPON_NEGEV: return			"negev";
		case ItemDefinitionIndex::WEAPON_P90: return			"p90";
		case ItemDefinitionIndex::WEAPON_P250: return			"p250";
		case ItemDefinitionIndex::WEAPON_REVOLVER: return		"revolver";
		case ItemDefinitionIndex::WEAPON_SAWEDOFF: return		"sawedoff";
		case ItemDefinitionIndex::WEAPON_SCAR20: return			"scar20";
		case ItemDefinitionIndex::WEAPON_SG556: return			"sg553";
		case ItemDefinitionIndex::WEAPON_SSG08: return			"ssg08";
		case ItemDefinitionIndex::WEAPON_TEC9: return			"tec9";
		case ItemDefinitionIndex::WEAPON_UMP45: return			"ump45";
		case ItemDefinitionIndex::WEAPON_USP_SILENCER: return	"usp";
		case ItemDefinitionIndex::WEAPON_XM1014: return			"xm1014";
		case ItemDefinitionIndex::WEAPON_TASER: return			"zeus";
		case ItemDefinitionIndex::WEAPON_KNIFE: return			"knife";
		case ItemDefinitionIndex::WEAPON_FLASHBANG: return		"flashbang";
		case ItemDefinitionIndex::WEAPON_HEGRENADE: return		"hegrenade";
		case ItemDefinitionIndex::WEAPON_SMOKEGRENADE: return	"smokegrenade";
		case ItemDefinitionIndex::WEAPON_MOLOTOV: return		"molotov";
		case ItemDefinitionIndex::WEAPON_DECOY: return			"decoy";
		case ItemDefinitionIndex::WEAPON_INCGRENADE: return		"incendiary";
		case ItemDefinitionIndex::WEAPON_C4: return				"c4";
		}
	}
};