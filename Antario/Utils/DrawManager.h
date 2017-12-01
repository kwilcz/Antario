#pragma once
#include <memory>
#include "D3DFont.h"


class DrawManager
{
public: // Function members
    // Basic non-drawing functions
    DrawManager();

    void Init       (LPDIRECT3DDEVICE9 pDevice);
    void Reset      (LPDIRECT3DDEVICE9 pDevice);
    void Invalidate ();

    // Drawing functions

    void DrawLine   (int x, int y, int x2, int y2, DWORD dwColor);
    void DrawRect   (int x, int y, int width, int height, DWORD dwColor);

    void DrawString (float x, float y, DWORD dwColor, const char* szText, DWORD dwFlags = 0L);

private: // Variable members
    LPDIRECT3DDEVICE9 pDevice;
    std::unique_ptr<CD3DFont> pFont;
};
extern DrawManager g_Render;
