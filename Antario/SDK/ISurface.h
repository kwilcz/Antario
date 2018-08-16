#pragma once
#include "..\Utils\Utils.h"

class ISurface
{
public:
	void UnlockCursor()
	{
		return Utils::CallVFunc<66, void>(this);
	}
};
extern ISurface* g_pSurface;