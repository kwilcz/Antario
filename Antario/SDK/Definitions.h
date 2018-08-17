#pragma once
#include <Windows.h>

typedef void*   (*CreateInterfaceFn)        (const char *pName, int *pReturnCode);
typedef void    (*pfnDemoCustomDataCallback)(unsigned char *pData, size_t iSize);

enum class EClassIds : int
{
    CAI_BaseNPC = 0,
    CAK47,
    CBaseAnimating,
    CBaseAnimatingOverlay,
    CBaseAttributableItem,
    CBaseButton,
    CBaseCombatCharacter,
    CBaseCombatWeapon,
    CBaseCSGrenade,
    CBaseCSGrenadeProjectile,
    CBaseDoor,
    CBaseEntity,
    CBaseFlex,
    CBaseGrenade,
    CBaseParticleEntity,
    CBasePlayer,
    CBasePropDoor,
    CBaseTeamObjectiveResource,
    CBaseTempEntity,
    CBaseToggle,
    CBaseTrigger,
    CBaseViewModel,
    CBaseVPhysicsTrigger,
    CBaseWeaponWorldModel,
    CBeam,
    CBeamSpotlight,
    CBoneFollower,
    CBreakableProp,
    CBreakableSurface,
    CC4,
    CCascadeLight,
    CChicken,
    CColorCorrection,
    CColorCorrectionVolume,
    CCSGameRulesProxy,
    CCSPlayer,
    CCSPlayerResource,
    CCSRagdoll,
    CCSTeam,
    CDEagle,
    CDecoyGrenade,
    CDecoyProjectile,
    CDynamicLight,
    CDynamicProp,
    CEconEntity,
    CEconWearable,
    CEmbers,
    CEntityDissolve,
    CEntityFlame,
    CEntityFreezing,
    CEntityParticleTrail,
    CEnvAmbientLight,
    CEnvDetailController,
    CEnvDOFController,
    CEnvParticleScript,
    CEnvProjectedTexture,
    CEnvQuadraticBeam,
    CEnvScreenEffect,
    CEnvScreenOverlay,
    CEnvTonemapController,
    CEnvWind,
    CFEPlayerDecal,
    CFireCrackerBlast,
    CFireSmoke,
    CFireTrail,
    CFish,
    CFlashbang,
    CFogController,
    CFootstepControl,
    CFunc_Dust,
    CFunc_LOD,
    CFuncAreaPortalWindow,
    CFuncBrush,
    CFuncConveyor,
    CFuncLadder,
    CFuncMonitor,
    CFuncMoveLinear,
    CFuncOccluder,
    CFuncReflectiveGlass,
    CFuncRotating,
    CFuncSmokeVolume,
    CFuncTrackTrain,
    CGameRulesProxy,
    CHandleTest,
    CHEGrenade,
    CHostage,
    CHostageCarriableProp,
    CIncendiaryGrenade,
    CInferno,
    CInfoLadderDismount,
    CInfoOverlayAccessor,
    CItem_Healthshot,
    CItemDogtags,
    CKnife,
    CKnifeGG,
    CLightGlow,
    CMaterialModifyControl,
    CMolotovGrenade,
    CMolotovProjectile,
    CMovieDisplay,
    CParticleFire,
    CParticlePerformanceMonitor,
    CParticleSystem,
    CPhysBox,
    CPhysBoxMultiplayer,
    CPhysicsProp,
    CPhysicsPropMultiplayer,
    CPhysMagnet,
    CPlantedC4,
    CPlasma,
    CPlayerResource,
    CPointCamera,
    CPointCommentaryNode,
    CPointWorldText,
    CPoseController,
    CPostProcessController,
    CPrecipitation,
    CPrecipitationBlocker,
    CPredictedViewModel,
    CProp_Hallucination,
    CPropDoorRotating,
    CPropJeep,
    CPropVehicleDriveable,
    CRagdollManager,
    CRagdollProp,
    CRagdollPropAttached,
    CRopeKeyframe,
    CSCAR17,
    CSceneEntity,
    CSensorGrenade,
    CSensorGrenadeProjectile,
    CShadowControl,
    CSlideshowDisplay,
    CSmokeGrenade,
    CSmokeGrenadeProjectile,
    CSmokeStack,
    CSpatialEntity,
    CSpotlightEnd,
    CSprite,
    CSpriteOriented,
    CSpriteTrail,
    CStatueProp,
    CSteamJet,
    CSun,
    CSunlightShadowControl,
    CTeam,
    CTeamplayRoundBasedRulesProxy,
    CTEArmorRicochet,
    CTEBaseBeam,
    CTEBeamEntPoint,
    CTEBeamEnts,
    CTEBeamFollow,
    CTEBeamLaser,
    CTEBeamPoints,
    CTEBeamRing,
    CTEBeamRingPoint,
    CTEBeamSpline,
    CTEBloodSprite,
    CTEBloodStream,
    CTEBreakModel,
    CTEBSPDecal,
    CTEBubbles,
    CTEBubbleTrail,
    CTEClientProjectile,
    CTEDecal,
    CTEDust,
    CTEDynamicLight,
    CTEEffectDispatch,
    CTEEnergySplash,
    CTEExplosion,
    CTEFireBullets,
    CTEFizz,
    CTEFootprintDecal,
    CTEFoundryHelpers,
    CTEGaussExplosion,
    CTEGlowSprite,
    CTEImpact,
    CTEKillPlayerAttachments,
    CTELargeFunnel,
    CTEMetalSparks,
    CTEMuzzleFlash,
    CTEParticleSystem,
    CTEPhysicsProp,
    CTEPlantBomb,
    CTEPlayerAnimEvent,
    CTEPlayerDecal,
    CTEProjectedDecal,
    CTERadioIcon,
    CTEShatterSurface,
    CTEShowLine,
    CTesla,
    CTESmoke,
    CTESparks,
    CTESprite,
    CTESpriteSpray,
    CTest_ProxyToggle_Networkable,
    CTestTraceline,
    CTEWorldDecal,
    CTriggerPlayerMovement,
    CTriggerSoundOperator,
    CVGuiScreen,
    CVoteController,
    CWaterBullet,
    CWaterLODControl,
    CWeaponAug,
    CWeaponAWP,
    CWeaponBaseItem,
    CWeaponBizon,
    CWeaponCSBase,
    CWeaponCSBaseGun,
    CWeaponCycler,
    CWeaponElite,
    CWeaponFamas,
    CWeaponFiveSeven,
    CWeaponG3SG1,
    CWeaponGalil,
    CWeaponGalilAR,
    CWeaponGlock,
    CWeaponHKP2000,
    CWeaponM249,
    CWeaponM3,
    CWeaponM4A1,
    CWeaponMAC10,
    CWeaponMag7,
    CWeaponMP5Navy,
    CWeaponMP7,
    CWeaponMP9,
    CWeaponNegev,
    CWeaponNOVA,
    CWeaponP228,
    CWeaponP250,
    CWeaponP90,
    CWeaponSawedoff,
    CWeaponSCAR20,
    CWeaponScout,
    CWeaponSG550,
    CWeaponSG552,
    CWeaponSG556,
    CWeaponSSG08,
    CWeaponTaser,
    CWeaponTec9,
    CWeaponTMP,
    CWeaponUMP45,
    CWeaponUSP,
    CWeaponXM1014,
    CWorld,
    DustTrail,
    MovieExplosion,
    ParticleSmokeGrenade,
    RocketTrail,
    SmokeTrail,
    SporeExplosion,
    SporeTrail
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

class WeaponInfo_t
{
public:
    void*           pVTable;
    char*	    	consoleName;			// 0x0004
    char	    	pad_0008[12];			// 0x0008
    int			    iMaxClip1;				// 0x0014
    int			    iMaxClip2;				// 0x0018
    int		    	iDefaultClip1;			// 0x001C
    int		    	iDefaultClip2;			// 0x0020
    char	    	pad_0024[8];			// 0x0024
    char*	    	szWorldModel;			// 0x002C
    char*	    	szViewModel;			// 0x0030
    char*	    	szDroppedModel;			// 0x0034
    char	    	pad_0038[4];			// 0x0038
    char*	    	N0000023E;				// 0x003C
    char	    	pad_0040[56];			// 0x0040
    char*	    	szEmptySound;			// 0x0078
    char	    	pad_007C[4];			// 0x007C
    char*	    	szBulletType;			// 0x0080
    char	    	pad_0084[4];			// 0x0084
    char*	    	szHudName;				// 0x0088
    char*	    	szWeaponName;			// 0x008C
    char		    pad_0090[56];			// 0x0090
    CSWeaponType    iWeaponType;			// 0x00C8
    int		    	iWeaponPrice;			// 0x00CC
    int		    	iKillAward;				// 0x00D0
    char*	    	szAnimationPrefix;		// 0x00D4
    float	    	flCycleTime;			// 0x00D8
    float	    	flCycleTimeAlt;			// 0x00DC
    float	    	flTimeToIdle;			// 0x00E0
    float	    	flIdleInterval;			// 0x00E4
    bool	    	bFullAuto;				// 0x00E8
    char	    	pad_0x00E5[3];			// 0x00E9
    int		    	iDamage;				// 0x00EC
    float	    	flArmorRatio;			// 0x00F0
    int		    	iBullets;				// 0x00F4
    float	    	flPenetration;			// 0x00F8
    float	    	flFlinchVelocityModifierLarge;	// 0x00FC
    float	    	flFlinchVelocityModifierSmall;	// 0x0100
    float	    	flRange;				// 0x0104
    float	    	flRangeModifier;		// 0x0108
    float	    	flThrowVelocity;		// 0x010C
    char		    pad_0x010C[12];			// 0x0110
    bool	    	bHasSilencer;			// 0x011C
    char	    	pad_0x0119[3];			// 0x011D
    char*	    	pSilencerModel;			// 0x0120
    int		    	iCrosshairMinDistance;	// 0x0124
    int		    	iCrosshairDeltaDistance;// 0x0128 - iTeam?
    float	    	flMaxPlayerSpeed;		// 0x012C
    float	    	flMaxPlayerSpeedAlt;	// 0x0130
    float	    	flSpread;				// 0x0134
    float	    	flSpreadAlt;			// 0x0138
    float	    	flInaccuracyCrouch;		// 0x013C
    float	    	flInaccuracyCrouchAlt;	// 0x0140
    float	    	flInaccuracyStand;		// 0x0144
    float	    	flInaccuracyStandAlt;	// 0x0148
    float	    	flInaccuracyJumpInitial;// 0x014C
    float	    	flInaccuracyJump;		// 0x0150
    float	    	flInaccuracyJumpAlt;	// 0x0154
    float	    	flInaccuracyLand;		// 0x0158
    float	    	flInaccuracyLandAlt;	// 0x015C
    float	    	flInaccuracyLadder;		// 0x0160
    float	    	flInaccuracyLadderAlt;	// 0x0164
    float	    	flInaccuracyFire;		// 0x0168
    float	    	flInaccuracyFireAlt;	// 0x016C
    float	    	flInaccuracyMove;		// 0x0170
    float	    	flInaccuracyMoveAlt;	// 0x0174
    float	    	flInaccuracyReload;		// 0x0178
    int		    	iRecoilSeed;			// 0x017C
    float	    	flRecoilAngle;			// 0x0180
    float	    	flRecoilAngleAlt;		// 0x0184
    float	    	flRecoilAngleVariance;	// 0x0188
    float	    	flRecoilAngleVarianceAlt;	// 0x018C
    float	    	flRecoilMagnitude;		// 0x0190
    float	    	flRecoilMagnitudeAlt;	// 0x0194
    float		    flRecoilMagnitudeVariance;	    // 0x0198
    float	    	flRecoilMagnitudeVarianceAlt;	// 0x019C
    float		    flRecoveryTimeCrouch;	// 0x01A0
    float	    	flRecoveryTimeStand;	// 0x01A4
    float	    	flRecoveryTimeCrouchFinal;	// 0x01A8
    float	    	flRecoveryTimeStandFinal;	// 0x01AC
    int		    	iRecoveryTransitionStartBullet; // 0x01B0 
    int		    	iRecoveryTransitionEndBullet;	// 0x01B4
    bool	    	bUnzoomAfterShot;		// 0x01B8
    bool	    	bHideViewModelZoomed;	// 0x01B9
    char	    	pad_0x01B5[2];			// 0x01BA
    char	    	iZoomLevels[3];			// 0x01BC
    int		    	iZoomFOV[2];			// 0x01C0
    float	    	fZoomTime[3];			// 0x01C4
    char*	    	szWeaponClass;			// 0x01D4
    float	    	flAddonScale;			// 0x01D8
    char	    	pad_0x01DC[4];			// 0x01DC
    char*	    	szEjectBrassEffect;		// 0x01E0
    char*		    szTracerEffect;			// 0x01E4
    int		       	iTracerFrequency;		// 0x01E8
    int		    	iTracerFrequencyAlt;	// 0x01EC
    char*	    	szMuzzleFlashEffect_1stPerson; // 0x01F0
    char	    	pad_0x01F4[4];			 // 0x01F4
    char*	    	szMuzzleFlashEffect_3rdPerson; // 0x01F8
    char	    	pad_0x01FC[4];			// 0x01FC
    char*	    	szMuzzleSmokeEffect;	// 0x0200
    float	    	flHeatPerShot;			// 0x0204
    char*	    	szZoomInSound;			// 0x0208
    char*	    	szZoomOutSound;			// 0x020C
    float	    	flInaccuracyPitchShift;	// 0x0210
    float	    	flInaccuracySoundThreshold;	// 0x0214
    float	    	flBotAudibleRange;		// 0x0218
    char	    	pad_0x0218[8];			// 0x0220
    char*	    	pWrongTeamMsg;			// 0x0224
    bool	    	bHasBurstMode;			// 0x0228
    char	    	pad_0x0225[3];			// 0x0229
    bool	    	bIsRevolver;			// 0x022C
    bool	    	bCannotShootUnderwater;	// 0x0230
};