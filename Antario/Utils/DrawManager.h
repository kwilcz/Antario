#pragma once
#include <memory>
#include "..\SDK\Color.h"
#include "..\SDK\Vector.h"
#include "D3DFont.h"

#define GET_D3DCOLOR_ALPHA(x) (( x >> 24) & 255)
/// TODO: Test this makro, if it works - Replace our function with it.
#define COL_TO_D3DCOLOR(x) (D3DCOLOR_ARGB(x.a, x.r, x.g, x.b))


class DrawManager
{
public: // Function members
    // Basic non-drawing functions

    DrawManager();

    void InitDeviceObjects      (LPDIRECT3DDEVICE9 pDevice);
    void RestoreDeviceObjects   (LPDIRECT3DDEVICE9 pDevice);
    void InvalidateDeviceObjects();


    // Drawing functions

    void Line   (Vector2D vecPos1, Vector2D vecPos2, Color color);
    void Line   (float posx1, float posy1, float posx2, float posy2, Color color);
    void Rect   (Vector2D vecPos1, Vector2D vecPos2, Color color);
    void Rect   (float posx, float posy, float width, float height, Color color);

    void String (float posx, float posy, DWORD dwFlags, Color color, CD3DFont* pFont, const char* szText, ...);


    // Helpers
    Vector2D GetScreenCenter();

private: // Variable members
    LPDIRECT3DDEVICE9   pDevice;
    D3DVIEWPORT9        pViewPort;

    D3DCOLOR ColorToD3DColor(Color color) { return D3DCOLOR_ARGB(color.a, color.r, color.g, color.b); }
};
extern DrawManager g_Render;

struct Fonts
{
public:
    void InvalidateDeviceObjects()
    {
        pFontTahoma8->InvalidateDeviceObjects();
        pFontTahoma12->InvalidateDeviceObjects();
    };
    void InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
    {

        pFontTahoma8->InitDeviceObjects(pDevice);
        pFontTahoma8->RestoreDeviceObjects();

        pFontTahoma12->InitDeviceObjects(pDevice);
        pFontTahoma12->RestoreDeviceObjects();
    };

    // Fonts
    std::unique_ptr<CD3DFont> pFontTahoma8;
    std::unique_ptr<CD3DFont> pFontTahoma12;
};
extern Fonts g_Fonts;
