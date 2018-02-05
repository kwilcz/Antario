#pragma once
#include "..\Utils\Utils.h"

#define MAX_SPLITSCREEN_PLAYERS 1

class CBaseHandle;
class C_BaseEntity;
class IClientEntity;
class CUserCmd;

class IMoveHelper
{
public:
    void SetHost(C_BaseEntity* host)
    {
        return Utils::CallVFunc<1, void>(this, host);
    }
};
extern IMoveHelper* g_pMoveHelper;

struct CMoveData { byte data[184]; };

class IGameMovement
{
public:
    virtual			~IGameMovement(void) {}

    virtual void	ProcessMovement(C_BaseEntity *pPlayer, CMoveData *pMove) = 0;
    virtual void	Reset(void) = 0;
    virtual void	StartTrackPredictionErrors(C_BaseEntity *pPlayer) = 0;
    virtual void	FinishTrackPredictionErrors(C_BaseEntity *pPlayer) = 0;
    virtual void	DiffPrint(char const *fmt, ...) = 0;

    virtual Vector const&	GetPlayerMins(bool ducked) const = 0;
    virtual Vector const&	GetPlayerMaxs(bool ducked) const = 0;
    virtual Vector const&   GetPlayerViewOffset(bool ducked) const = 0;

    virtual bool			IsMovingPlayerStuck(void) const = 0;
    virtual C_BaseEntity*	GetMovingPlayer(void) const = 0;
    virtual void			UnblockPusher(C_BaseEntity* pPlayer, C_BaseEntity *pPusher) = 0;

    virtual void    SetupMovementBounds(CMoveData *pMove) = 0;
};
extern IGameMovement* g_pMovement;

class CPrediction
{
    // Construction
public:

    virtual ~CPrediction(void) = 0;//

    virtual void Init(void) = 0;//
    virtual void Shutdown(void) = 0;//

                                    // Implement IPrediction
public:

    virtual void Update
    (
        int startframe, // World update ( un-modded ) most recently received
        bool validframe, // Is frame data valid
        int incoming_acknowledged, // Last command acknowledged to have been run by server (un-modded)
        int outgoing_command // Last command (most recent) sent to server (un-modded)
    );//

    virtual void PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet);//
    virtual void PostEntityPacketReceived(void);//5
    virtual void PostNetworkDataReceived(int commands_acknowledged);//

    virtual void OnReceivedUncompressedPacket(void);//

                                                    // The engine needs to be able to access a few predicted values
    virtual void GetViewOrigin(Vector& org);//
    virtual void SetViewOrigin(Vector& org);//
    virtual void GetViewAngles(Vector& ang);//10
    virtual void SetViewAngles(Vector& ang);//

    virtual void GetLocalViewAngles(Vector& ang);//
    virtual void SetLocalViewAngles(Vector& ang);//

    virtual bool InPrediction(void) const;//14
    virtual bool IsFirstTimePredicted(void) const;//

    virtual int GetLastAcknowledgedCommandNumber(void) const;//

#if !defined( NO_ENTITY_PREDICTION )
    virtual int GetIncomingPacketNumber(void) const;//
#endif
    
    virtual void CheckMovingGround(IClientEntity* player, double frametime);//
    virtual void RunCommand(IClientEntity* player, CUserCmd* cmd, IMoveHelper* moveHelper);//

    virtual void SetupMove(C_BaseEntity* player, CUserCmd* cmd, IMoveHelper* pHelper, CMoveData* move);//20
    virtual void FinishMove(C_BaseEntity* player, CUserCmd* cmd, CMoveData* move);//
    virtual void SetIdealPitch(int nSlot, IClientEntity* player, const Vector& origin, const Vector& angles, const Vector& viewheight);//

    virtual void CheckError(int nSlot, IClientEntity* player, int commands_acknowledged);//
    
public:
    virtual void _Update
    (
        int nSlot,
        bool received_new_world_update,
        bool validframe,            // Is frame data valid
        int incoming_acknowledged,  // Last command acknowledged to have been run by server (un-modded)
        int outgoing_command        // Last command (most recent) sent to server (un-modded)
    );

    // Actually does the prediction work, returns false if an error occurred
    bool PerformPrediction(int nSlot, IClientEntity* localPlayer, bool received_new_world_update, int incoming_acknowledged, int outgoing_command);

    void ShiftIntermediateDataForward(int nSlot, int slots_to_remove, int previous_last_slot);

    void RestoreEntityToPredictedFrame(int nSlot, int predicted_frame);

    int ComputeFirstCommandToExecute(int nSlot, bool received_new_world_update, int incoming_acknowledged, int outgoing_command);

    void DumpEntity(IClientEntity* ent, int commands_acknowledged);

    void ShutdownPredictables(void);

    void ReinitPredictables(void);

    void RemoveStalePredictedEntities(int nSlot, int last_command_packet);

    void RestoreOriginalEntityState(int nSlot);

    void RunSimulation(int current_command, float curtime, CUserCmd* cmd, IClientEntity* localPlayer);

    void Untouch(int nSlot);

    void StorePredictionResults(int nSlot, int predicted_frame);

    bool ShouldDumpEntity(IClientEntity* ent);

    void SmoothViewOnMovingPlatform(IClientEntity* pPlayer, Vector& offset);

    void ResetSimulationTick();

    void ShowPredictionListEntry(int listRow, int showlist, IClientEntity* ent, int& totalsize, int& totalsize_intermediate);

    void FinishPredictionList(int listRow, int showlist, int totalsize, int totalsize_intermediate);

    void CheckPredictConvar();

#if !defined( NO_ENTITY_PREDICTION )

#endif
};

extern CPrediction* g_pPrediction;