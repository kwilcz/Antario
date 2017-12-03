#pragma once
#include <memory>
#include "..\SDK\Vector.h"
#include "D3DFont.h"

#define GET_D3DCOLOR_ALPHA(x) (( x >> 24) & 255)


class DrawManager
{
public: // Function members
    // Basic non-drawing functions
    DrawManager();

    void Init       (LPDIRECT3DDEVICE9 pDevice);
    void Reset      (LPDIRECT3DDEVICE9 pDevice);
    void Invalidate ();

    // Drawing functions

    void DrawLine   (Vector2D vecPos1, Vector2D vecPos2, DWORD dwColor);
    void DrawLine   (float x, float y, float x2, float y2, DWORD dwColor);
    void DrawRect   (Vector2D vecPos1, Vector2D vecPos2, DWORD dwColor);
    void DrawRect   (float x, float y, float width, float height, DWORD dwColor);

    void DrawString (float x, float y, DWORD dwFlags, DWORD dwColor, const char* szText, ...);

private: // Variable members
    LPDIRECT3DDEVICE9 pDevice;
    std::unique_ptr<CD3DFont> pFont;
};
extern DrawManager g_Render;
