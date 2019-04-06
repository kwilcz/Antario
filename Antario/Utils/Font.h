#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#include <string>
#include <map>

#include "SPoint.h"
#include "Color.h"

#include <ft2build.h>

#include FT_FREETYPE_H
#include FT_STROKER_H

#pragma comment (lib, "d3dx9")      // link D3DX DLL
#pragma comment (lib, "freetype")   // link freetype DLL

// Releasing makro making sure we dont try to release a null pointer
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } }

enum FontFlags : int
{ 
    FONT_NONE = 0,
    FONT_CENTERED_X = (1 << 0),
    FONT_CENTERED_Y = (1 << 1),
    FONT_DROPSHADOW = (1 << 2)
};

struct GlyphInfo
{
    SPoint size;       /* width/height of the glyph */
    SPoint bearing;    /* x and y - offsets from baseline to left / top of glyph*/
    uintptr_t advance; /* width + bearing */
    LPDIRECT3DTEXTURE9 texture; /* pointer to the glyph texture */
    bool colored;   /* if the glyph has its own color */
};

class Font
{
public:
    Font() = delete;
    Font(const char* strFontName, int height, bool bAntialias, LPDIRECT3DDEVICE9 pDevice, int outlineThickness = 0);

    void Release();
    void OnLostDevice();
    void OnResetDevice(LPDIRECT3DDEVICE9  pDevice);
    template <typename T>
    void Render(const T* strToRender, SPoint ptPos, DWORD flags, Color color = Color::White(), float scale = 1.f);
    template <typename T>
    void Render(const T& strToRender, SPoint ptPos, DWORD flags, Color color = Color::White(), float scale = 1.f);
    template <typename T>
    SPoint GetTextDimensions(const T& str, bool bDropShadow = false);

    int iHeight;
private:
    template <typename T>
    void CreateCharTexture(T ch);
    void GenerateAsciiChars();
    void SetupRenderStates();
    std::string GetFontPath(const char* strFontName);

    int iOutlineThickness;
    LPDIRECT3DDEVICE9  pDevice;
    LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;

    LPDIRECT3DSTATEBLOCK9 pStateBlockRender;
    LPDIRECT3DSTATEBLOCK9 pStateBlockOld;

    FT_Library  ftLibrary;
    FT_Face     ftFace;
    FT_Stroker  ftStroker;

    bool bIsAntialiased;
    std::map<int, GlyphInfo> mapGlyphs;
};

