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

    void FireGameEvent(IGameEvent* event)
    {
        // call functions here
        if(strstr(event->GetName(), "player_hurt")){
        	int hurt_player_id = event->GetInt("userid");
	        int attacker_id = event->GetInt("attacker");
            //big copypaste!!!!
            //i wrote this on browser, dont really know if syntaxes beyond this point are even correct
            if(attacker_id == g::pLocalEntity->EntIndex()){
            //do hitmarkers / player->shotshit++ / etc.
            //you can also use this to switch head side like aIMwARE.cc does
            }
            
        }
    }

    int GetEventDebugID() override
    {
        return EVENT_DEBUG_ID_INIT;
    }
};
