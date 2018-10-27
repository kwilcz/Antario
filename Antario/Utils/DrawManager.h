#pragma once
#include <memory>
#include "SRect.h"      // Includes both SPoint and SRect
#include "D3DFont.h"
#include "..\Utils\Color.h"
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
    void SetupRenderStates      () const;


    // Drawing functions

    void Line(SPoint vecPos1, SPoint vecPos2, Color color) const;
    void Line(int posx1, int posy1, int posx2, int posy2, Color color) const;

    void Rect(SRect rcBouds, Color color) const;
    void Rect(SPoint vecPos1, SPoint vecPos2, Color color) const;
    void Rect(int posx1, int posy1, int posx2, int posy2, Color color) const;

    void RectBordered(SRect rcBouds, Color color) const;
    void RectBordered(SPoint vecPos1, SPoint vecPos2, Color color) const;
    void RectBordered(int posx1, int posy1, int posx2, int posy2, Color color) const;

    void RectFilled(SRect rcPosition, Color color) const;
    void RectFilled(SPoint vecPos1, SPoint vecPos2, Color color) const;
    void RectFilled(int posx1, int posy1, int posx2, int posy2, Color color) const;

    void Triangle(SPoint pos1, SPoint pos2, SPoint pos3, Color color) const;
    void TriangleFilled(SPoint pos1, SPoint pos2, SPoint pos3, Color color) const;

    void RectFilledGradient(SRect rcBoudingRect, Color col1, Color col2, GradientType type) const;
    void RectFilledGradient(SPoint vecPos1, SPoint vecPos2, Color col1, Color col2, GradientType type) const;
    void RectFilledGradient(int posx1, int posy1, int posx2, int posy2, Color col1, Color col2, GradientType vertical) const;

    void RectFilledGradientMultiColor(SRect rcBoudingRect, Color colTopLeft, Color colTopRight, Color colBottomLeft, Color colBottomRight) const;
    void RectFilledGradientMultiColor(SPoint vecPos1, SPoint vecPos2, Color colTopLeft, Color colTopRight, Color colBottomLeft, Color colBottomRight) const;
    void RectFilledGradientMultiColor(int posx1, int posy1, int posx2, int posy2, Color colTopLeft, Color colTopRight, Color colBottomLeft, Color colBottomRight) const;

    void String(SPoint vecPos, DWORD dwFlags, Color color, CD3DFont * pFont, const char * szText) const;
    void String(int posx, int posy, DWORD dwFlags, Color color, CD3DFont* pFont, const char* szText) const;

    // Helpers
    SPoint            GetScreenCenter() const;
    D3DVIEWPORT9      GetViewport()     const { D3DVIEWPORT9 tmpVp; pDevice->GetViewport(&tmpVp); return tmpVp; }
    LPDIRECT3DDEVICE9 GetRenderDevice() const { return pDevice; }
    void SetCustomViewport(const D3DVIEWPORT9& pNewViewport);
    void SetCustomViewport(const SRect& vpRect);

    void SetCustomScissorRect(const SRect& rcRect);
    void RestoreOriginalScissorRect() const;
    void RestoreOriginalViewport();

private: // Variable members
    LPDIRECT3DDEVICE9 pDevice;
    D3DVIEWPORT9      pViewPort;
    RECT              pScissorRect;
};
extern DrawManager g_Render;


///TODO: Change these logs
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
    std::shared_ptr<CD3DFont> pFontTahoma8;
    std::shared_ptr<CD3DFont> pFontTahoma10;
};
extern Fonts g_Fonts;


enum GradientType
{
    GRADIENT_VERTICAL,
    GRADIENT_HORIZONTAL
};
