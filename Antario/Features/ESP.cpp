#include "ESP.h"
#include "..\Utils\Utils.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\PlayerInfo.h"

ESP g_ESP;

void ESP::RenderName(C_BaseEntity* pEnt, int iterator)
{
    Color textColor;
    if (pEnt->GetTeam() == g::pLocalEntity->GetTeam())
        textColor = Color(195, 240, 100, 255);
    else
        textColor = Color(250, 165, 110, 255);


    PlayerInfo_t pInfo;
    g_pEngine->GetPlayerInfo(iterator, &pInfo);
    std::string strPlayerName = std::string(pInfo.szName);


    Vector vecPosition = pEnt->GetEyePosition(); vecPosition.z += 30;
    Vector vecScreenPos;
    if (!Utils::WorldToScreen(vecPosition, vecScreenPos))
        return;

    g_Render.String(vecScreenPos.x, vecScreenPos.y, CD3DFONT_CENTERED_X, textColor, g_Fonts.pFontTahoma10.get(), strPlayerName.c_str());
}


void ESP::Render()
{
    for (int it = 0; it < g_pEngine->GetMaxClients(); ++it)
    {
        C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(it);

        // Sanity checks
        if (!pPlayerEntity
            || !pPlayerEntity->IsAlive()
            || pPlayerEntity->IsDormant()
            || pPlayerEntity == g::pLocalEntity)
            continue;

        this->RenderName(pPlayerEntity, it);
    }
}
