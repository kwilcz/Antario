#pragma once

#include "Font.h"
#include "SRect.h"      // Includes both SPoint and SRect
#include "..\Utils\Color.h"
#include "..\Utils\Utils.h"

#include <memory>
#include <queue>

#define GET_D3DCOLOR_ALPHA(x) (( x >> 24) & 255)
#define COL2DWORD(x) (D3DCOLOR_ARGB(x.alpha, x.red, x.green, x.blue))

enum GradientType;

class DrawManager
{
public: // Function members
    // Basic non-drawing functions

    DrawManager();

    void InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice);
    void OnLostDevice();
    void OnResetDevice(LPDIRECT3DDEVICE9 pDevice);
    void Release();
    void SetupRenderStates() const;


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

    template <typename... Targs>
    void String(SPoint ptPos, DWORD dwFlags, Color color, std::shared_ptr<Font> pFont, const char * szText, Targs... args) const;
    template <typename... Targs>
    void String(int posx, int posy, DWORD dwFlags, Color color, std::shared_ptr<Font> pFont, const char* szText, Targs... args) const;
    template <typename... Targs>
    void String(SPoint ptPos, DWORD dwFlags, Color color, float scale, std::shared_ptr<Font> pFont, const char * szText, Targs... args) const;
    template <typename... Targs>
    void String(int posx, int posy, DWORD dwFlags, Color color, float scale, std::shared_ptr<Font> pFont, const char* szText, Targs... args) const;


    // Helpers
    SPoint            GetScreenCenter() const;
    D3DVIEWPORT9      GetViewport()     const { D3DVIEWPORT9 tmpVp; pDevice->GetViewport(&tmpVp); return tmpVp; }
    LPDIRECT3DDEVICE9 GetRenderDevice() const { return pDevice; }
    void SetCustomViewport(const D3DVIEWPORT9& pNewViewport);
    void SetCustomViewport(const SRect& vpRect);

    void SetCustomScissorRect(const SRect& rcRect);
    void RestoreOriginalScissorRect();
    void RestoreOriginalViewport();

private: // Variable members
    LPDIRECT3DDEVICE9 pDevice;
    D3DVIEWPORT9      pViewPort;
    SPoint            szScreenSize;
    std::deque<RECT>  pScissorRect{};
};
extern DrawManager g_Render;


/* fonts */

enum FontNames : int
{
    FONT_TAHOMA_8 = 0,
    FONT_TAHOMA_10 = 1
};

///TODO: Change these logs
struct Fonts
{
public:
    void Release()
    {
        Utils::Log("Font: Release");
        try
        {
            for (auto& ft : vecFonts)
                ft->Release();
        }
        catch (const HRESULT& hr)
        {
            Utils::Log("Font: Release failed.");
            Utils::Log(hr);
        }
    };
    void OnLostDevice()
    {
        Utils::Log("Font: OnLostDevice");
        try
        {
            for (auto& ft : vecFonts)
                ft->OnLostDevice();
        }
        catch (const HRESULT& hr)
        {
            Utils::Log("Font: OnLostDevice failed.");
            Utils::Log(hr);
        }
    };
    void OnResetDevice(LPDIRECT3DDEVICE9 pDevice)
    {
        Utils::Log("Font: OnResetDevice");
        try
        {
            for (auto& ft : vecFonts)
                ft->OnResetDevice(pDevice);
        }
        catch (const HRESULT& hr)
        {
            Utils::Log("Font: OnResetDevice failed.");
            Utils::Log(hr);
        }
    };

    // Fonts
    std::vector<std::shared_ptr<Font>> vecFonts;
};
extern Fonts g_Fonts;


enum GradientType
{
    GRADIENT_VERTICAL,
    GRADIENT_HORIZONTAL
};


template <typename... Targs>
void DrawManager::String(SPoint ptPos, DWORD dwFlags, Color color, std::shared_ptr<Font> pFont, const char* szText, Targs... args) const
{
    const std::string retString = Utils::SetupStringParams(szText, args...);
    pFont->Render(retString, ptPos, dwFlags, color);
}

template <typename... Targs>
void DrawManager::String(int posx, int posy, DWORD dwFlags, Color color, std::shared_ptr<Font> pFont, const char* szText, Targs... args) const
{
    const std::string retString = Utils::SetupStringParams(szText, args...);
    pFont->Render(retString, { posx , posy }, dwFlags, color);
}

template <typename... Targs>
void DrawManager::String(SPoint ptPos, DWORD dwFlags, Color color, float scale, std::shared_ptr<Font> pFont, const char * szText, Targs... args) const
{
    const std::string retString = Utils::SetupStringParams(szText, args...);
    pFont->Render(retString, ptPos, dwFlags, color);
}

template <typename... Targs>
void DrawManager::String(int posx, int posy, DWORD dwFlags, Color color, float scale, std::shared_ptr<Font> pFont, const char* szText, Targs... args) const
{
    const std::string retString = Utils::SetupStringParams(szText, args...);
    pFont->Render(retString, { posx , posy }, dwFlags, color, scale);
}