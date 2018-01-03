#include "ESP.h"
#include "..\Utils\Utils.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\PlayerInfo.h"

ESP g_ESP;

void ESP::RenderName(C_BaseEntity* pEnt, int iterator)
{
	PlayerInfo_t pInfo;
	g_pEngine->GetPlayerInfo(iterator, &pInfo);


	Vector vecPosition = pEnt->GetEyePosition(); vecPosition.z += 30;
	Vector vecScreenPos;
	if (!Utils::WorldToScreen(vecPosition, vecScreenPos))
		return;

	g_Render.String(vecScreenPos.x, vecScreenPos.y,
		CD3DFONT_CENTERED_X | CD3DFONT_DROPSHADOW,
		(localTeam == pEnt->GetTeam()) ? teamColor : enemyColor,
		g_Fonts.pFontTahoma10.get(), pInfo.szName);
}


void ESP::Render()
{
	localTeam = g::pLocalEntity->GetTeam();

	for (int it = 0; it < g_pEngine->GetMaxClients(); ++it)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(it);

		// Sanity checks
		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == g::pLocalEntity)
			continue;

		if (g_Settings.bShowNames)
			this->RenderName(pPlayerEntity, it);
	}
}