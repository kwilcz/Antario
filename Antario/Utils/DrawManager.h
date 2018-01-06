#pragma once
#include <memory>
#include "D3DFont.h"
#include "..\SDK\Color.h"
#include "..\SDK\Vector.h"
#include "..\Utils\Utils.h"

#define GET_D3DCOLOR_ALPHA(x) (( x >> 24) & 255)
#define COL2DWORD(x) (D3DCOLOR_ARGB(x.alpha, x.red, x.green, x.blue))

enum GradientType;

class DrawManager
{
public: // Function members
    // Basic non-drawing functions

    DrawManager();

    void InitDeviceObjects      (LPDIRECT3DDEVICE9 pDevice);
    void RestoreDeviceObjects   (LPDIRECT3DDEVICE9 pDevice);
    void InvalidateDeviceObjects();
    void SetupRenderStates      ();


    // Drawing functions

    void Line   (Vector2D vecPos1, Vector2D vecPos2, Color color) const;
    void Line   (float posx1, float posy1, float posx2, float posy2, Color color) const;
    void Rect   (Vector2D vecPos1, Vector2D vecPos2, Color color) const;
    void Rect   (float posx1, float posy1, float posx2, float posy2, Color color) const;
    void RectFilled         (Vector2D vecPos1, Vector2D vecPos2, Color color) const;
    void RectFilled         (float posx1, float posy1, float posx2, float posy2, Color color) const;
    void Triangle           (Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color) const;
    void TriangleFilled     (Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color) const;
    void RectFilledGradient (Vector2D vecPos1, Vector2D vecPos2, Color col1, Color col2, GradientType type) const;

    void String (float posx, float posy, DWORD dwFlags, Color color, CD3DFont* pFont, const char* szText, ...) const;


    // Helpers
    Vector2D GetScreenCenter();

private: // Variable members
    LPDIRECT3DDEVICE9   pDevice;
    D3DVIEWPORT9        pViewPort;

};
extern DrawManager g_Render;

struct Fonts
{
public:
    void DeleteDeviceObjects()
    {
        Utils::Log("Deleting device objects...");
        HRESULT hr;
        hr = pFontTahoma8->DeleteDeviceObjects();
        if (FAILED(hr)) { Utils::Log("Deleting proceture failed for font Tahoma 8"); }
        hr = pFontTahoma10->DeleteDeviceObjects();
        if (FAILED(hr)) { Utils::Log("Deleting proceture failed for font Tahoma 10"); }
    };
    void InvalidateDeviceObjects()
    {
        HRESULT hr;
        hr = pFontTahoma8->InvalidateDeviceObjects();
        if (FAILED(hr)) { Utils::Log("Invalidating failed for font Tahoma 8"); }
        hr = pFontTahoma10->InvalidateDeviceObjects();
        if (FAILED(hr)) { Utils::Log("Invalidating failed for font Tahoma 10"); }
    };
    void InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
    {

        HRESULT hr;
        hr = pFontTahoma8->InitDeviceObjects(pDevice);
        if (FAILED(hr)) { Utils::Log(hr); }
        hr = pFontTahoma8->RestoreDeviceObjects();
        if (FAILED(hr)) { Utils::Log(hr); }

        hr = pFontTahoma10->InitDeviceObjects(pDevice);
        if (FAILED(hr)) { Utils::Log(hr); }
        hr = pFontTahoma10->RestoreDeviceObjects();
        if (FAILED(hr)) { Utils::Log(hr); }
    };

    // Fonts
    std::unique_ptr<CD3DFont> pFontTahoma8;
    std::unique_ptr<CD3DFont> pFontTahoma10;
};
extern Fonts g_Fonts;


enum GradientType
{
    GRADIENT_VERTICAL,
    GRADIENT_HORIZONTAL
};