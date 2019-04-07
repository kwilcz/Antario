#pragma once
#include <Windows.h>

typedef void*   (*CreateInterfaceFn)        (const char *pName, int *pReturnCode);
typedef void    (*pfnDemoCustomDataCallback)(unsigned char *pData, size_t iSize);

enum class EClassIds : int
{
    CAI_BaseNPC = 0,
    CAK47 = 1,
    CBaseAnimating = 2,
    CBaseAnimatingOverlay = 3,
    CBaseAttributableItem = 4,
    CBaseButton = 5,
    CBaseCombatCharacter = 6,
    CBaseCombatWeapon = 7,
    CBaseCSGrenade = 8,
    CBaseCSGrenadeProjectile = 9,
    CBaseDoor = 10,
    CBaseEntity = 11,
    CBaseFlex = 12,
    CBaseGrenade = 13,
    CBaseParticleEntity = 14,
    CBasePlayer = 15,
    CBasePropDoor = 16,
    CBaseTeamObjectiveResource = 17,
    CBaseTempEntity = 18,
    CBaseToggle = 19,
    CBaseTrigger = 20,
    CBaseViewModel = 21,
    CBaseVPhysicsTrigger = 22,
    CBaseWeaponWorldModel = 23,
    CBeam = 24,
    CBeamSpotlight = 25,
    CBoneFollower = 26,
    CBRC4Target = 27,
    CBreachCharge = 28,
    CBreachChargeProjectile = 29,
    CBreakableProp = 30,
    CBreakableSurface = 31,
    CC4 = 32,
    CCascadeLight = 33,
    CChicken = 34,
    CColorCorrection = 35,
    CColorCorrectionVolume = 36,
    CCSGameRulesProxy = 37,
    CCSPlayer = 38,
    CCSPlayerResource = 39,
    CCSRagdoll = 40,
    CCSTeam = 41,
    CDangerZone = 42,
    CDangerZoneController = 43,
    CDEagle = 44,
    CDecoyGrenade = 45,
    CDecoyProjectile = 46,
    CDrone = 47,
    CDronegun = 48,
    CDynamicLight = 49,
    CDynamicProp = 50,
    CEconEntity = 51,
    CEconWearable = 52,
    CEmbers = 53,
    CEntityDissolve = 54,
    CEntityFlame = 55,
    CEntityFreezing = 56,
    CEntityParticleTrail = 57,
    CEnvAmbientLight = 58,
    CEnvDetailController = 59,
    CEnvDOFController = 60,
    CEnvGasCanister = 61,
    CEnvParticleScript = 62,
    CEnvProjectedTexture = 63,
    CEnvQuadraticBeam = 64,
    CEnvScreenEffect = 65,
    CEnvScreenOverlay = 66,
    CEnvTonemapController = 67,
    CEnvWind = 68,
    CFEPlayerDecal = 69,
    CFireCrackerBlast = 70,
    CFireSmoke = 71,
    CFireTrail = 72,
    CFish = 73,
    CFists = 74,
    CFlashbang = 75,
    CFogController = 76,
    CFootstepControl = 77,
    CFunc_Dust = 78,
    CFunc_LOD = 79,
    CFuncAreaPortalWindow = 80,
    CFuncBrush = 81,
    CFuncConveyor = 82,
    CFuncLadder = 83,
    CFuncMonitor = 84,
    CFuncMoveLinear = 85,
    CFuncOccluder = 86,
    CFuncReflectiveGlass = 87,
    CFuncRotating = 88,
    CFuncSmokeVolume = 89,
    CFuncTrackTrain = 90,
    CGameRulesProxy = 91,
    CGrassBurn = 92,
    CHandleTest = 93,
    CHEGrenade = 94,
    CHostage = 95,
    CHostageCarriableProp = 96,
    CIncendiaryGrenade = 97,
    CInferno = 98,
    CInfoLadderDismount = 99,
    CInfoMapRegion = 100,
    CInfoOverlayAccessor = 101,
    CItem_Healthshot = 102,
    CItemCash = 103,
    CItemDogtags = 104,
    CKnife = 105,
    CKnifeGG = 106,
    CLightGlow = 107,
    CMaterialModifyControl = 108,
    CMelee = 109,
    CMolotovGrenade = 110,
    CMolotovProjectile = 111,
    CMovieDisplay = 112,
    CParadropChopper = 113,
    CParticleFire = 114,
    CParticlePerformanceMonitor = 115,
    CParticleSystem = 116,
    CPhysBox = 117,
    CPhysBoxMultiplayer = 118,
    CPhysicsProp = 119,
    CPhysicsPropMultiplayer = 120,
    CPhysMagnet = 121,
    CPhysPropAmmoBox = 122,
    CPhysPropLootCrate = 123,
    CPhysPropRadarJammer = 124,
    CPhysPropWeaponUpgrade = 125,
    CPlantedC4 = 126,
    CPlasma = 127,
    CPlayerResource = 128,
    CPointCamera = 129,
    CPointCommentaryNode = 130,
    CPointWorldText = 131,
    CPoseController = 132,
    CPostProcessController = 133,
    CPrecipitation = 134,
    CPrecipitationBlocker = 135,
    CPredictedViewModel = 136,
    CProp_Hallucination = 137,
    CPropCounter = 138,
    CPropDoorRotating = 139,
    CPropJeep = 140,
    CPropVehicleDriveable = 141,
    CRagdollManager = 142,
    CRagdollProp = 143,
    CRagdollPropAttached = 144,
    CRopeKeyframe = 145,
    CSCAR17 = 146,
    CSceneEntity = 147,
    CSensorGrenade = 148,
    CSensorGrenadeProjectile = 149,
    CShadowControl = 150,
    CSlideshowDisplay = 151,
    CSmokeGrenade = 152,
    CSmokeGrenadeProjectile = 153,
    CSmokeStack = 154,
    CSnowball = 155,
    CSnowballPile = 156,
    CSnowballProjectile = 157,
    CSpatialEntity = 158,
    CSpotlightEnd = 159,
    CSprite = 160,
    CSpriteOriented = 161,
    CSpriteTrail = 162,
    CStatueProp = 163,
    CSteamJet = 164,
    CSun = 165,
    CSunlightShadowControl = 166,
    CSurvivalSpawnChopper = 167,
    CTablet = 168,
    CTeam = 169,
    CTeamplayRoundBasedRulesProxy = 170,
    CTEArmorRicochet = 171,
    CTEBaseBeam = 172,
    CTEBeamEntPoint = 173,
    CTEBeamEnts = 174,
    CTEBeamFollow = 175,
    CTEBeamLaser = 176,
    CTEBeamPoints = 177,
    CTEBeamRing = 178,
    CTEBeamRingPoint = 179,
    CTEBeamSpline = 180,
    CTEBloodSprite = 181,
    CTEBloodStream = 182,
    CTEBreakModel = 183,
    CTEBSPDecal = 184,
    CTEBubbles = 185,
    CTEBubbleTrail = 186,
    CTEClientProjectile = 187,
    CTEDecal = 188,
    CTEDust = 189,
    CTEDynamicLight = 190,
    CTEEffectDispatch = 191,
    CTEEnergySplash = 192,
    CTEExplosion = 193,
    CTEFireBullets = 194,
    CTEFizz = 195,
    CTEFootprintDecal = 196,
    CTEFoundryHelpers = 197,
    CTEGaussExplosion = 198,
    CTEGlowSprite = 199,
    CTEImpact = 200,
    CTEKillPlayerAttachments = 201,
    CTELargeFunnel = 202,
    CTEMetalSparks = 203,
    CTEMuzzleFlash = 204,
    CTEParticleSystem = 205,
    CTEPhysicsProp = 206,
    CTEPlantBomb = 207,
    CTEPlayerAnimEvent = 208,
    CTEPlayerDecal = 209,
    CTEProjectedDecal = 210,
    CTERadioIcon = 211,
    CTEShatterSurface = 212,
    CTEShowLine = 213,
    CTesla = 214,
    CTESmoke = 215,
    CTESparks = 216,
    CTESprite = 217,
    CTESpriteSpray = 218,
    CTest_ProxyToggle_Networkable = 219,
    CTestTraceline = 220,
    CTEWorldDecal = 221,
    CTriggerPlayerMovement = 222,
    CTriggerSoundOperator = 223,
    CVGuiScreen = 224,
    CVoteController = 225,
    CWaterBullet = 226,
    CWaterLODControl = 227,
    CWeaponAug = 228,
    CWeaponAWP = 229,
    CWeaponBaseItem = 230,
    CWeaponBizon = 231,
    CWeaponCSBase = 232,
    CWeaponCSBaseGun = 233,
    CWeaponCycler = 234,
    CWeaponElite = 235,
    CWeaponFamas = 236,
    CWeaponFiveSeven = 237,
    CWeaponG3SG1 = 238,
    CWeaponGalil = 239,
    CWeaponGalilAR = 240,
    CWeaponGlock = 241,
    CWeaponHKP2000 = 242,
    CWeaponM249 = 243,
    CWeaponM3 = 244,
    CWeaponM4A1 = 245,
    CWeaponMAC10 = 246,
    CWeaponMag7 = 247,
    CWeaponMP5Navy = 248,
    CWeaponMP7 = 249,
    CWeaponMP9 = 250,
    CWeaponNegev = 251,
    CWeaponNOVA = 252,
    CWeaponP228 = 253,
    CWeaponP250 = 254,
    CWeaponP90 = 255,
    CWeaponSawedoff = 256,
    CWeaponSCAR20 = 257,
    CWeaponScout = 258,
    CWeaponSG550 = 259,
    CWeaponSG552 = 260,
    CWeaponSG556 = 261,
    CWeaponSSG08 = 262,
    CWeaponTaser = 263,
    CWeaponTec9 = 264,
    CWeaponTMP = 265,
    CWeaponUMP45 = 266,
    CWeaponUSP = 267,
    CWeaponXM1014 = 268,
    CWorld = 269,
    CWorldVguiText = 270,
    DustTrail = 271,
    MovieExplosion = 272,
    ParticleSmokeGrenade = 273,
    RocketTrail = 274,
    SmokeTrail = 275,
    SporeExplosion = 276,
};

enum class MoveType_t
{
    MOVETYPE_NONE = 0,
    MOVETYPE_ISOMETRIC,
    MOVETYPE_WALK,
    MOVETYPE_STEP,
    MOVETYPE_FLY,
    MOVETYPE_FLYGRAVITY,
    MOVETYPE_VPHYSICS,
    MOVETYPE_PUSH,
    MOVETYPE_NOCLIP,
    MOVETYPE_LADDER,
    MOVETYPE_OBSERVER,
    MOVETYPE_CUSTOM,
    MOVETYPE_LAST = MOVETYPE_CUSTOM,
    MOVETYPE_MAX_BITS = 4
};

enum EntityFlags : int
{
    FL_ONGROUND =   (1 << 0),
    FL_DUCKING =    (1 << 1),
    FL_WATERJUMP =  (1 << 2),
    FL_ONTRAIN =    (1 << 3),
    FL_INRAIN =     (1 << 4),
    FL_FROZEN =     (1 << 5),
    FL_ATCONTROLS = (1 << 6),
    FL_CLIENT =     (1 << 7),
    FL_FAKECLIENT = (1 << 8)
};

enum class ItemDefinitionIndex : short 
{
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_MP5 = 23,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516
};

enum class CSWeaponType : int
{
    WEAPONTYPE_KNIFE = 0,
    WEAPONTYPE_PISTOL,
    WEAPONTYPE_SUBMACHINEGUN,
    WEAPONTYPE_RIFLE,
    WEAPONTYPE_SHOTGUN,
    WEAPONTYPE_SNIPER_RIFLE,
    WEAPONTYPE_MACHINEGUN,
    WEAPONTYPE_C4,
    WEAPONTYPE_PLACEHOLDER,
    WEAPONTYPE_GRENADE,
    WEAPONTYPE_UNKNOWN
};

class WeaponInfo_t {
public:
	void *pVTable;                       // 0x0000
	char *consoleName;                   // 0x0004
	char pad_0008[12];                   // 0x0008
	int iMaxClip1;                       // 0x0014
	int iMaxClip2;                       // 0x0018
	int iDefaultClip1;                   // 0x001C
	int iDefaultClip2;                   // 0x0020
	char pad_0024[8];                    // 0x0024
	char *szWorldModel;                  // 0x002C
	char *szViewModel;                   // 0x0030
	char *szDroppedMode;                 // 0x0034
	char pad_0038[4];                    // 0x0038
	char *szShotSound;                   // 0x003C
	char pad_0040[56];                   // 0x0040
	char *szEmptySound;                  // 0x0078
	char pad_007C[4];                    // 0x007C
	char *szBulletType;                  // 0x0080
	char pad_0084[4];                    // 0x0084
	char *szHudName;                     // 0x0088
	char *szWeaponName;                  // 0x008C
	char pad_0090[56];                   // 0x0090
	CSWeaponType iWeaponType;            // 0x00C8
	char pad_00CC[4];                    // 0x00CC
	int iWeaponPrice;                    // 0x00D0
	int iKillAward;                      // 0x00D4
	char *szAnimationPrefix;             // 0x00D8
	float flCycleTime;                   // 0x00DC
	float flCycleTimeAlt;                // 0x00E0
	float flTimeToIdle;                  // 0x00E4
	float flIdleInterval;                // 0x00E8
	bool bFullAuto;                      // 0x00EC
	char pad_00ED[3];                    // 0x00ED
	int iDamage;                         // 0x00F0
	float flArmorRatio;                  // 0x00F4
	int iBullets;                        // 0x00F8
	float flPenetration;                 // 0x00FC
	float flFlinchVelocityModifierLarge; // 0x0100
	float flFlinchVelocityModifierSmall; // 0x0104
	float flRange;                       // 0x0108
	float flRangeModifier;               // 0x010C
	float flThrowVelocity;               // 0x0110
	char pad_0114[12];                   // 0x0114
	bool bHasSilencer;                   // 0x0120
	char pad_0121[3];                    // 0x0121
	char *pSilencerModel;                // 0x0124
	int iCrosshairMinDistance;           // 0x0128
	int iCrosshairDeltaDistance;         // 0x012C
	float flMaxPlayerSpeed;              // 0x0130
	float flMaxPlayerSpeedAlt;           // 0x0134
	float flMaxPlayerSpeedMod;           // 0x0138
	float flSpread;                      // 0x013C
	float flSpreadAlt;                   // 0x0140
	float flInaccuracyCrouch;            // 0x0144
	float flInaccuracyCrouchAlt;         // 0x0148
	float flInaccuracyStand;             // 0x014C
	float flInaccuracyStandAlt;          // 0x0150
	float flInaccuracyJumpInitial;       // 0x0154
	float flInaccuracyJump;              // 0x0158
	float flInaccuracyJumpAlt;           // 0x015C
	float flInaccuracyLand;              // 0x0160
	float flInaccuracyLandAlt;           // 0x0164
	float flInaccuracyLadder;            // 0x0168
	float flInaccuracyLadderAlt;         // 0x016C
	float flInaccuracyFire;              // 0x0170
	float flInaccuracyFireAlt;           // 0x0174
	float flInaccuracyMove;              // 0x0178
	float flInaccuracyMoveAlt;           // 0x017C
	float flInaccuracyReload;            // 0x0180
	int iRecoilSeed;                     // 0x0184
	float flRecoilAngle;                 // 0x0188
	float flRecoilAngleAlt;              // 0x018C
	float flRecoilAngleVariance;         // 0x0190
	float flRecoilAngleVarianceAlt;      // 0x0194
	float flRecoilMagnitude;             // 0x0198
	float flRecoilMagnitudeAlt;          // 0x019C
	float flRecoilMagnitudeVariance;     // 0x01A0
	float flRecoilMagnitudeVarianceAlt;  // 0x01A4
	float flRecoveryTimeCrouch;          // 0x01A8
	float flRecoveryTimeStand;           // 0x01AC
	float flRecoveryTimeCrouchFinal;     // 0x01B0
	float flRecoveryTimeStandFinal;      // 0x01B4
	char pad_01B8[40];                   // 0x01B8
	char *szWeaponClass;                 // 0x01E0
	char pad_01E4[8];                    // 0x01E4
	char *szEjectBrassEffect;            // 0x01EC
	char *szTracerEffect;                // 0x01F0
	int iTracerFrequency;                // 0x01F4
	int iTracerFrequencyAlt;             // 0x01F8
	char *szMuzzleFlashEffect_1stPerson; // 0x01FC
	char pad_0200[4];                    // 0x0200
	char *szMuzzleFlashEffect_3rdPerson; // 0x0204
	char pad_0208[4];                    // 0x0208
	char *szMuzzleSmokeEffect;           // 0x020C
	float flHeatPerShot;                 // 0x0210
	char *szZoomInSound;                 // 0x0214
	char *szZoomOutSound;                // 0x0218
	float flInaccuracyPitchShift;        // 0x021C
	float flInaccuracySoundThreshold;    // 0x0220
	float flBotAudibleRange;             // 0x0224
	char pad_0228[12];                   // 0x0228
	bool bHasBurstMode;                  // 0x0234
	bool bIsRevolver;                    // 0x0235
};                                     // Size: 0x0236

class CWeaponSystem
{
    virtual void placeholder0();
    virtual void placeholder1();
public:
    virtual WeaponInfo_t* GetWpnData(ItemDefinitionIndex idx);
};