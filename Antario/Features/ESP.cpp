#include "ESP.h"
#include "..\Settings.h"
#include "..\Utils\Utils.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\PlayerInfo.h"

ESP g_ESP;

void ESP::RenderBox(C_BaseEntity* pEnt)
{
    Vector vecScreenOrigin, vecOrigin = pEnt->GetRenderOrigin();
    if (!Utils::WorldToScreen(vecOrigin, vecScreenOrigin))
        return;

    Vector vecScreenBottom, vecBottom = vecOrigin;
    vecBottom.z += (pEnt->GetFlags() & FL_DUCKING) ? 54.f : 72.f;
    if (!Utils::WorldToScreen(vecBottom, vecScreenBottom))
        return;

    const auto sx = int(std::roundf(vecScreenOrigin.x)),
               sy = int(std::roundf(vecScreenOrigin.y)),
               h  = int(std::roundf(vecScreenBottom.y - vecScreenOrigin.y)),
               w  = int(std::roundf(h * 0.25f));

    /* Draw rect around the entity */
    g_Render.Rect(sx - w, sy, sx + w, sy + h, (pEnt->GetTeam() == localTeam) ? teamColor : enemyColor);

    /* Draw rect outline */
    g_Render.Rect(sx - w - 1, sy - 1, sx + w + 1, sy + h + 1, Color::Black());
    g_Render.Rect(sx - w + 1, sy + 1, sx + w - 1, sy + h - 1, Color::Black());
}

void ESP::RenderName(C_BaseEntity* pEnt, int iterator)
{
    Vector vecScreenOrigin, 
           vecOrigin = pEnt->GetRenderOrigin();

    if (!Utils::WorldToScreen(vecOrigin, vecScreenOrigin))
        return;

    Vector vecScreenBottom, vecBottom = vecOrigin;
    vecBottom.z += (pEnt->GetFlags() & FL_DUCKING) ? 54.f : 72.f;
    if (!Utils::WorldToScreen(vecBottom, vecScreenBottom))
        return;


    PlayerInfo_t pInfo;
    g_pEngine->GetPlayerInfo(iterator, &pInfo);

    const auto sx = int(std::roundf(vecScreenOrigin.x)),
               sy = int(std::roundf(vecScreenOrigin.y)),
               h  = int(std::roundf(vecScreenBottom.y - vecScreenOrigin.y));

    g_Render.String(sx, sy + h - 16, FONT_CENTERED_X | FONT_DROPSHADOW,
                    (localTeam == pEnt->GetTeam()) ? teamColor : enemyColor,
                    g_Fonts.vecFonts[FONT_TAHOMA_10], pInfo.szName);
}

void ESP::RenderWeaponName(C_BaseEntity* pEnt)
{
    Vector vecScreenOrigin, vecOrigin = pEnt->GetRenderOrigin();
    if (!Utils::WorldToScreen(vecOrigin, vecScreenOrigin))
        return;

    auto weapon = pEnt->GetActiveWeapon();
    if (!weapon)
        return;

    auto strWeaponName = weapon->GetName();

    /* Remove "weapon_" prefix */
    strWeaponName.erase(0, 7); 
    /* All uppercase */
    std::transform(strWeaponName.begin(), strWeaponName.end(), strWeaponName.begin(), ::toupper);

    g_Render.String(int(vecScreenOrigin.x), int(vecScreenOrigin.y), FONT_CENTERED_X | FONT_DROPSHADOW,
                    (localTeam == pEnt->GetTeam()) ? teamColor : enemyColor,
                    g_Fonts.vecFonts[FONT_TAHOMA_10], strWeaponName.c_str());
}


void ESP::Render()
{
    if (!g::pLocalEntity || !g_pEngine->IsInGame())
        return;

    localTeam = g::pLocalEntity->GetTeam();

    for (int it = 1; it <= g_pEngine->GetMaxClients(); ++it)
    {
        C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(it);
        
        if (!pPlayerEntity
            || pPlayerEntity == g::pLocalEntity
            || pPlayerEntity->IsDormant()
            || !pPlayerEntity->IsAlive())
            continue;
        
        if (g_Settings.bShowBoxes)
            this->RenderBox(pPlayerEntity);

        if (g_Settings.bShowNames)
            this->RenderName(pPlayerEntity, it);

        if (g_Settings.bShowWeapons)
            this->RenderWeaponName(pPlayerEntity);
    }
}
