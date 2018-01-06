#pragma once
#include <vector>
#include "SDK\IGameEvent.h"

class EventListener : public IGameEventListener2
{
public:
    EventListener(std::vector<const char*> events)
    {
        for (auto& it : events)
            g_pEventManager->AddListener(this, it, false);
    }

    ~EventListener()
    {
        g_pEventManager->RemoveListener(this);
    }

    void FireGameEvent(IGameEvent* event) override
    {
        // call functions here
    }

    int GetEventDebugID() override
    {
        return EVENT_DEBUG_ID_INIT;
    }
};