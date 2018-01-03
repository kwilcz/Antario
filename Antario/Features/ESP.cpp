#include "ESP.h"
#include "..\Utils\Utils.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\PlayerInfo.h"

ESP g_ESP;

void ESP::RenderBox(C_BaseEntity* pEnt)
{
	auto origin = pEnt->GetOrigin();
	auto top = origin;
	origin.z += (pEnt->GetFlags() & FL_DUCKING) ? 54.f : 72.f;

	Vector screenOrigin;
	if (!Utils::WorldToScreen(origin, screenOrigin))
		return;

	Vector screenTop;
	if (!Utils::WorldToScreen(top, screenTop))
		return;


	auto x = screenTop.x;
	auto y = screenTop.y;
	auto h = screenOrigin.y - screenTop.y;
	auto w = h / 4;

	g_Render.Rect(x - w, y, x + w, y + h, (pEnt->GetTeam() == localTeam) ? teamColor : enemyColor);

	//outline
	g_Render.Rect(x - w - 1, y - 1, x + w + 1, y + h + 1, Color(0, 0, 0, 255));
	g_Render.Rect(x - w + 1, y + 1, x + w - 1, y + h - 1, Color(0, 0, 0, 255));
}

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

	for (int it = 1; it <= g_pEngine->GetMaxClients(); ++it)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(it);

		// Sanity checks
		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == g::pLocalEntity)
			continue;

		if (g_Settings.bShowBoxes)
			this->RenderBox(pPlayerEntity);

		if (g_Settings.bShowNames)
			this->RenderName(pPlayerEntity, it);
	}
}
