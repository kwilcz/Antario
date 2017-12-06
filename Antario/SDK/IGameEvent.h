#pragma once
#include "KeyValues.h"
#include "CInput.h"

#define EVENT_DEBUG_ID_INIT 42
#define EVENT_DEBUG_ID_SHUTDOWN 13

class IGameEvent
{
public:
    virtual ~IGameEvent() {};
    virtual const char *GetName() const = 0;                // get event name

    virtual bool  IsReliable() const = 0;                   // if event handled reliable
    virtual bool  IsLocal() const = 0;                      // if event is never networked
    virtual bool  IsEmpty(const char *keyName = NULL) = 0;  // check if data field exists

                                                            // Data access
    virtual bool  GetBool(const char *keyName = NULL, bool defaultValue = false) = 0;
    virtual int   GetInt(const char *keyName = NULL, int defaultValue = 0) = 0;
    virtual unsigned long long GetUint64(char const *keyName = NULL, unsigned long long defaultValue = 0) = 0;
    virtual float GetFloat(const char *keyName = NULL, float defaultValue = 0.0f) = 0;

    virtual const char *GetString(const char *keyName = NULL, const char *defaultValue = "") = 0;
    virtual const wchar_t *GetWString(char const *keyName = NULL, const wchar_t *defaultValue = L"") = 0;

    virtual void SetBool(const char *keyName, bool value) = 0;
    virtual void SetInt(const char *keyName, int value) = 0;
    virtual void SetUInt64(const char *keyName, unsigned long long value) = 0;
    virtual void SetFloat(const char *keyName, float value) = 0;
    virtual void SetString(const char *keyName, const char *value) = 0;
    virtual void SetWString(const char *keyName, const wchar_t *value) = 0;
};

class IGameEventListener2
{
public:
    virtual ~IGameEventListener2(void) {};

    virtual void FireGameEvent(IGameEvent *event) = 0;
    virtual int GetEventDebugID(void) = 0;
};

class IGameEventManager2
{
public:
    virtual ~IGameEventManager2(void) {};
    virtual int LoadEventsFromFile(const char* filename) = 0;
    virtual void Reset() = 0;
    virtual bool AddListener(IGameEventListener2* listener, const char* name, bool serverside) = 0;
    virtual bool FindListener(IGameEventListener2* listener, const char* name) = 0;
    virtual void RemoveListener(IGameEventListener2* listener) = 0;
    virtual void AddListenerGlobal(IGameEventListener2* listener, bool serverside) = 0;
    virtual IGameEvent* CreateEvent(const char* name, bool force = false, int* cookie = nullptr) = 0;
    virtual bool FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;
    virtual bool FireEventClientSide(IGameEvent* event) = 0;
    virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;
    virtual void FreeEvent(IGameEvent* event) = 0;
    virtual bool SerializeEvent(IGameEvent* event, bf_write* buffer) = 0;
    virtual IGameEvent* UnserializeEvent(bf_read* buffer) = 0;
    virtual KeyValues* GetEventDataTypes(IGameEvent* event) = 0;
};
extern IGameEventManager2* g_pEventManager;