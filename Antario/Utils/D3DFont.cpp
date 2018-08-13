//-----------------------------------------------------------------------------
// File: D3DFont.cpp
//
// Desc: Texture-based font class
//-----------------------------------------------------------------------------

#pragma once
#include <cstdio>
#include <cmath>
#include <tchar.h>

#include "D3DFont.h"




//-----------------------------------------------------------------------------
// Custom vertex types for rendering text
//-----------------------------------------------------------------------------
#define MAX_NUM_VERTICES (50*6)

struct FONT2DVERTEX
{
    D3DXVECTOR4 p;
    DWORD       color;
    FLOAT       tu, tv;
};

struct FONT3DVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;
};

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex(const D3DXVECTOR4& p, D3DCOLOR color, FLOAT tu, FLOAT tv)
{
    FONT2DVERTEX v;
    v.p     = p;
    v.color = color;
    v.tu    = tu;
    v.tv    = tv;
    return v;
}

inline FONT3DVERTEX InitFont3DVertex(const D3DXVECTOR3& p, const D3DXVECTOR3& n, FLOAT tu, FLOAT tv)
{
    FONT3DVERTEX v;
    v.p  = p;
    v.n  = n;
    v.tu = tu;
    v.tv = tv;
    return v;
}




//-----------------------------------------------------------------------------
// Name: CD3DFont()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CD3DFont::CD3DFont(const TCHAR* strFontName, DWORD dwHeight, DWORD dwWeight, DWORD dwFlags)
{
    wcsncpy_s(this->strFontName, strFontName, sizeof(this->strFontName) / sizeof(TCHAR));
    this->strFontName[sizeof(this->strFontName) / sizeof(TCHAR) - 1] = _T('\0');
    this->dwFontHeight      = dwHeight;
    this->dwFontWeight      = dwWeight;
    this->dwFontFlags       = dwFlags;
    this->dwSpacing         = 0;

    this->pd3dDevice = NULL;
    this->pTexture   = NULL;
    this->pVB        = NULL;

    this->pStateBlockSaved      = NULL;
    this->pStateBlockDrawString = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont()
{
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
    HRESULT hr;

    // Keep a local copy of the device
    this->pd3dDevice = pd3dDevice;

    // Establish the font and texture size
    this->fTextScale = 1.0f; // Draw fonts into texture without scaling

    // Large fonts need larger textures
    if (this->dwFontHeight > 60)
        this->dwTexWidth = this->dwTexHeight = 2048;
    else if (this->dwFontHeight > 30)
        this->dwTexWidth = this->dwTexHeight = 1024;
    else if (this->dwFontHeight > 15)
        this->dwTexWidth = this->dwTexHeight = 512;
    else
        this->dwTexWidth = this->dwTexHeight = 256;

    // If requested texture is too big, use a smaller texture and smaller font,
    // and scale up when rendering.
    D3DCAPS9 d3dCaps;
    this->pd3dDevice->GetDeviceCaps(&d3dCaps);

    if (this->dwTexWidth > d3dCaps.MaxTextureWidth)
    {
        this->fTextScale = FLOAT(d3dCaps.MaxTextureWidth) / FLOAT(this->dwTexWidth);
        this->dwTexWidth = this->dwTexHeight = d3dCaps.MaxTextureWidth;
    }

    // Create a new texture for the font
    hr = this->pd3dDevice->CreateTexture(this->dwTexWidth, this->dwTexHeight, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED,
                                         &this->pTexture, NULL);
    if (FAILED(hr))
        return hr;

    // Prepare to create a bitmap
    DWORD*     pBitmapBits;
    BITMAPINFO bmi;
    ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bmi.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth  = int(this->dwTexWidth);
    bmi.bmiHeader.biHeight = -int(this->dwTexHeight);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount    = 32;

    // Create a DC and a bitmap for the font
    HDC     hDC       = CreateCompatibleDC(NULL);
    HBITMAP hbmBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&pBitmapBits, NULL, 0);

    // Sanity checks
    if (hDC == NULL)
        return E_FAIL;
    if (hbmBitmap == NULL)
        return E_FAIL;

    SetMapMode(hDC, MM_TEXT);

    // Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
    // antialiased font, but this is not guaranteed.
    INT   nHeight  = -MulDiv(this->dwFontHeight, (INT)(GetDeviceCaps(hDC, LOGPIXELSY) * this->fTextScale), 72);
    DWORD dwItalic = (this->dwFontFlags & D3DFONT_ITALIC) ? TRUE : FALSE;
    HFONT hFont    = CreateFont(nHeight, 0, 0, 0, this->dwFontWeight, dwItalic,FALSE, FALSE, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                this->dwFontHeight > 8 ? CLEARTYPE_NATURAL_QUALITY : ANTIALIASED_QUALITY,VARIABLE_PITCH,
                                this->strFontName);

    if (NULL == hFont)
        return E_FAIL;

    HGDIOBJ hbmOld   = SelectObject(hDC, hbmBitmap);
    HGDIOBJ hFontOld = SelectObject(hDC, hFont);

    // Set text properties
    SetTextColor(hDC, RGB(255, 255, 255));
    SetBkColor(hDC, 0x00000000);
    SetTextAlign(hDC, TA_TOP);

    // Loop through all printable character and output them to the bitmap..
    // Meanwhile, keep track of the corresponding tex coords for each character.
    DWORD x      = 0;
    DWORD y      = 0;
    TCHAR str[2] = _T("x");
    SIZE  size;

    // Calculate the spacing between characters based on line height
    GetTextExtentPoint32(hDC, TEXT(" "), 1, &size);
    x = this->dwSpacing = (DWORD)ceil(size.cy * 0.3f);

    for (TCHAR c = 32; c < 127; c++)
    {
        str[0] = c;
        GetTextExtentPoint32(hDC, str, 1, &size);

        if ((DWORD)(x + size.cx + this->dwSpacing) > this->dwTexWidth)
        {
            x = this->dwSpacing;
            y += size.cy + 1;
        }

        ExtTextOut(hDC, x + 0, y + 0, ETO_OPAQUE, NULL, str, 1, NULL);

        this->fTexCoords[c - 32][0] = ((FLOAT)(x + 0 - this->dwSpacing)) / this->dwTexWidth;
        this->fTexCoords[c - 32][1] = ((FLOAT)(y + 0 + 0)) / this->dwTexHeight;
        this->fTexCoords[c - 32][2] = ((FLOAT)(x + size.cx + this->dwSpacing)) / this->dwTexWidth;
        this->fTexCoords[c - 32][3] = ((FLOAT)(y + size.cy + 0)) / this->dwTexHeight;

        x += size.cx + (2 * this->dwSpacing);
    }

    // Lock the surface and write the alpha values for the set pixels
    D3DLOCKED_RECT d3dlr;
    this->pTexture->LockRect(0, &d3dlr, 0, 0);
    BYTE* pDstRow = (BYTE*)d3dlr.pBits;
    WORD* pDst16;
    BYTE  bAlpha; // 4-bit measure of pixel intensity

    for (y = 0; y < this->dwTexHeight; y++)
    {
        pDst16 = (WORD*)pDstRow;
        for (x = 0; x < this->dwTexWidth; x++)
        {
            bAlpha = (BYTE)((pBitmapBits[this->dwTexWidth * y + x] & 0xff) >> 4);
            if (bAlpha > 0)
            {
                *pDst16++ = (WORD)((bAlpha << 12) | 0x0fff);
            }
            else
            {
                *pDst16++ = 0x0000;
            }
        }
        pDstRow += d3dlr.Pitch;
    }

    // Done updating texture, so clean up used objects
    this->pTexture->UnlockRect(0);
    SelectObject(hDC, hbmOld);
    SelectObject(hDC, hFontOld);
    DeleteObject(hbmBitmap);
    DeleteObject(hFont);
    DeleteDC(hDC);

    this->iHeight = static_cast<float>([this]()
    {
        SIZE size;
        this->GetTextExtent("WJ", &size);
        return size.cy;
    }());

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DFont::RestoreDeviceObjects()
{
    HRESULT hr;

    // Create vertex buffer for the letters
    int vertexSize = max(sizeof(FONT2DVERTEX), sizeof(FONT3DVERTEX));
    if (FAILED(hr = this->pd3dDevice->CreateVertexBuffer(MAX_NUM_VERTICES * vertexSize,
        D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
        D3DPOOL_DEFAULT, &this->pVB, NULL)))
    {
        return hr;
    }

    // Create the state blocks for rendering text
    for (UINT which = 0; which < 2; which++)
    {
        this->pd3dDevice->BeginStateBlock();
        this->pd3dDevice->SetTexture(0, this->pTexture);

        if (D3DFONT_ZENABLE & this->dwFontFlags)
            this->pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
        else
            this->pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

        this->pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        this->pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        this->pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        this->pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        this->pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
        this->pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
        this->pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        this->pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        this->pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
        this->pd3dDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
        this->pd3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
        this->pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
        this->pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
        this->pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
        this->pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
                                         D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
                                         D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
        this->pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        this->pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        this->pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        this->pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        this->pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        this->pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        this->pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
        this->pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
        this->pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        this->pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        this->pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        this->pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        this->pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

        if (which == 0)
            this->pd3dDevice->EndStateBlock(&this->pStateBlockSaved);
        else
            this->pd3dDevice->EndStateBlock(&this->pStateBlockDrawString);
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InvalidateDeviceObjects()
{
    SAFE_RELEASE(this->pVB);
    SAFE_RELEASE(this->pStateBlockSaved);
    SAFE_RELEASE(this->pStateBlockDrawString);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DeleteDeviceObjects()
{
    SAFE_RELEASE(this->pTexture);
    this->pd3dDevice = NULL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
HRESULT CD3DFont::GetTextExtent(const char* strText, SIZE* pSize)
{
    if (NULL == strText || NULL == pSize)
        return E_FAIL;

    FLOAT fRowWidth  = 0.0f;
    FLOAT fRowHeight = (this->fTexCoords[0][3] - this->fTexCoords[0][1]) * this->dwTexHeight;
    FLOAT fWidth     = 0.0f;
    FLOAT fHeight    = fRowHeight;

    while (*strText)
    {
        TCHAR c = *strText++;

        if (c == _T('\n'))
        {
            fRowWidth = 0.0f;
            fHeight += fRowHeight;
        }

        if ((c - 32) < 0 || (c - 32) >= 128 - 32)
            continue;

        FLOAT tx1 = this->fTexCoords[c - 32][0];
        FLOAT tx2 = this->fTexCoords[c - 32][2];

        fRowWidth += (tx2 - tx1) * this->dwTexWidth - 2 * this->dwSpacing;

        if (fRowWidth > fWidth)
            fWidth = fRowWidth;
    }

    pSize->cx = (int)fWidth;
    pSize->cy = (int)fHeight;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawStringScaled()
// Desc: Draws scaled 2D text.  Note that x and y are in viewport coordinates
//       (ranging from -1 to +1).  fXScale and fYScale are the size fraction 
//       relative to the entire viewport.  For example, a fXScale of 0.25 is
//       1/8th of the screen width.  This allows you to output text at a fixed
//       fraction of the viewport, even if the screen or window size changes.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawStringScaled(FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, DWORD dwColor, const char* strText,
                                   DWORD dwFlags)
{
    if (this->pd3dDevice == NULL)
        return E_FAIL;

    // Set up renderstate
    this->pStateBlockSaved->Capture();
    this->pStateBlockDrawString->Apply();
    this->pd3dDevice->SetFVF(D3DFVF_FONT2DVERTEX);
    this->pd3dDevice->SetPixelShader(NULL);
    this->pd3dDevice->SetStreamSource(0, this->pVB, 0, sizeof(FONT2DVERTEX));

    // Set filter states
    if (dwFlags & CD3DFONT_FILTERED)
    {
        this->pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        this->pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    }

    D3DVIEWPORT9 vp;
    this->pd3dDevice->GetViewport(&vp);
    FLOAT fLineHeight = (this->fTexCoords[0][3] - this->fTexCoords[0][1]) * this->dwTexHeight;

    // Center the text block
    if (dwFlags & CD3DFONT_CENTERED_X)
    {
        SIZE sz;
        GetTextExtent(strText, &sz);
        x = -(((FLOAT)sz.cx)) * 0.5f;
        x = std::roundf(x);
    }

    if (dwFlags & CD3DFONT_CENTERED_Y)
    {
        SIZE sz;
        GetTextExtent(strText, &sz);
        y = -(((FLOAT)sz.cy)) * 0.5f;
        y = std::roundf(y);
    }

    FLOAT sx = (x + 1.0f) * vp.Width / 2;
    FLOAT sy = (y + 1.0f) * vp.Height / 2;

    // Adjust for character spacing
    sx -= this->dwSpacing * (fXScale * vp.Height) / fLineHeight;
    FLOAT fStartX = sx;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices;
    DWORD         dwNumTriangles = 0L;
    this->pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

    while (*strText)
    {
        TCHAR c = *strText++;

        if (c == _T('\n'))
        {
            sx = fStartX;
            sy += fYScale * vp.Height;
        }

        if ((c - 32) < 0 || (c - 32) >= 128 - 32)
            continue;

        FLOAT tx1 = this->fTexCoords[c - 32][0];
        FLOAT ty1 = this->fTexCoords[c - 32][1];
        FLOAT tx2 = this->fTexCoords[c - 32][2];
        FLOAT ty2 = this->fTexCoords[c - 32][3];

        FLOAT w = (tx2 - tx1) * this->dwTexWidth;
        FLOAT h = (ty2 - ty1) * this->dwTexHeight;

        w *= (fXScale * vp.Height) / fLineHeight;
        h *= (fYScale * vp.Height) / fLineHeight;

        if (c != _T(' '))
        {
            if (dwFlags & CD3DFONT_DROPSHADOW)
            {
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 + 0.5f, sy + h + 0.5f, 1.0f, 1.0f), 0x9a000000, tx1, ty2);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 + 0.5f, sy + 0 + 0.5f, 1.0f, 1.0f), 0x9a000000, tx1, ty1);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w + 0.5f, sy + h + 0.5f, 1.0f, 1.0f), 0x9a000000, tx2, ty2);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w + 0.5f, sy + 0 + 0.5f, 1.0f, 1.0f), 0x9a000000, tx2, ty1);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w + 0.5f, sy + h + 0.5f, 1.0f, 1.0f), 0x9a000000, tx2, ty2);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 + 0.5f, sy + 0 + 0.5f, 1.0f, 1.0f), 0x9a000000, tx1, ty1);
                dwNumTriangles += 2;
            }

            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + h - 0.5f, 1.0f, 1.0f), dwColor, tx1, ty2);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + 0 - 0.5f, 1.0f, 1.0f), dwColor, tx1, ty1);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + h - 0.5f, 1.0f, 1.0f), dwColor, tx2, ty2);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + 0 - 0.5f, 1.0f, 1.0f), dwColor, tx2, ty1);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + h - 0.5f, 1.0f, 1.0f), dwColor, tx2, ty2);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + 0 - 0.5f, 1.0f, 1.0f), dwColor, tx1, ty1);
            dwNumTriangles += 2;

            if (dwNumTriangles * 3 > (MAX_NUM_VERTICES - 6))
            {
                // Unlock, render, and relock the vertex buffer
                this->pVB->Unlock();
                this->pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);
                this->pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
                dwNumTriangles = 0L;
            }
        }

        sx += w - (2 * this->dwSpacing) * (fXScale * vp.Height) / fLineHeight;
    }

    // Unlock and render the vertex buffer
    this->pVB->Unlock();
    if (dwNumTriangles > 0)
        this->pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);

    // Restore the modified renderstates
    this->pStateBlockSaved->Apply();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawString()
// Desc: Draws 2D text. Note that sx and sy are in pixels
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawString(FLOAT sx, FLOAT sy, DWORD dwColor, const char* strText, DWORD dwFlags)
{
    if (this->pd3dDevice == NULL)
        return E_FAIL;

    // Setup renderstate
    this->pStateBlockSaved->Capture();
    this->pStateBlockDrawString->Apply();
    this->pd3dDevice->SetFVF(D3DFVF_FONT2DVERTEX);
    this->pd3dDevice->SetPixelShader(NULL);
    this->pd3dDevice->SetStreamSource(0, this->pVB, 0, sizeof(FONT2DVERTEX));

    // Set filter states
    if (dwFlags & CD3DFONT_FILTERED)
    {
        this->pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        this->pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    }

    // Center the text block
    if (dwFlags & CD3DFONT_CENTERED_X)
    {
        SIZE sz;
        GetTextExtent(strText, &sz);
        sx -= (FLOAT)sz.cx * 0.5f;
        sx = std::roundf(sx);
    }

    if (dwFlags & CD3DFONT_CENTERED_Y)
    {
        SIZE sz;
        GetTextExtent(strText, &sz);
        sy -= (FLOAT)sz.cy * 0.5f;
        sy = std::roundf(sy);
    }

    // Adjust for character spacing
    sx -= this->dwSpacing;
    FLOAT fStartX = sx;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices      = NULL;
    DWORD         dwNumTriangles = 0;
    this->pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

    while (*strText)
    {
        TCHAR c = *strText++;

        if (c == _T('\n'))
        {
            sx = fStartX;
            sy += (this->fTexCoords[0][3] - this->fTexCoords[0][1]) * this->dwTexHeight;
        }

        if ((c - 32) < 0 || (c - 32) >= 128 - 32)
            continue;

        FLOAT tx1 = this->fTexCoords[c - 32][0];
        FLOAT ty1 = this->fTexCoords[c - 32][1];
        FLOAT tx2 = this->fTexCoords[c - 32][2];
        FLOAT ty2 = this->fTexCoords[c - 32][3];

        FLOAT w = (tx2 - tx1) * this->dwTexWidth / this->fTextScale;
        FLOAT h = (ty2 - ty1) * this->dwTexHeight / this->fTextScale;

        if (c != _T(' '))
        {
            if (dwFlags & CD3DFONT_DROPSHADOW)
            {
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 + 0.5f, sy + h + 0.5f, 1.0f, 1.0f), 0x9a000000, tx1, ty2);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 + 0.5f, sy + 0 + 0.5f, 1.0f, 1.0f), 0x9a000000, tx1, ty1);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w + 0.5f, sy + h + 0.5f, 1.0f, 1.0f), 0x9a000000, tx2, ty2);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w + 0.5f, sy + 0 + 0.5f, 1.0f, 1.0f), 0x9a000000, tx2, ty1);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w + 0.5f, sy + h + 0.5f, 1.0f, 1.0f), 0x9a000000, tx2, ty2);
                *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 + 0.5f, sy + 0 + 0.5f, 1.0f, 1.0f), 0x9a000000, tx1, ty1);
                dwNumTriangles += 2;
            }

            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + h - 0.5f, 1.0f, 1.0f), dwColor, tx1, ty2);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + 0 - 0.5f, 1.0f, 1.0f), dwColor, tx1, ty1);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + h - 0.5f, 1.0f, 1.0f), dwColor, tx2, ty2);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + 0 - 0.5f, 1.0f, 1.0f), dwColor, tx2, ty1);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + w - 0.5f, sy + h - 0.5f, 1.0f, 1.0f), dwColor, tx2, ty2);
            *pVertices++ = InitFont2DVertex(D3DXVECTOR4(sx + 0 - 0.5f, sy + 0 - 0.5f, 1.0f, 1.0f), dwColor, tx1, ty1);
            dwNumTriangles += 2;

            if (dwNumTriangles * 3 > (MAX_NUM_VERTICES - 6))
            {
                // Unlock, render, and relock the vertex buffer
                this->pVB->Unlock();
                this->pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);
                pVertices = NULL;
                this->pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
                dwNumTriangles = 0L;
            }
        }

        sx += w - (2 * this->dwSpacing);
    }

    // Unlock and render the vertex buffer
    this->pVB->Unlock();
    if (dwNumTriangles > 0)
        this->pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, dwNumTriangles);

    // Restore the modified renderstates
    this->pStateBlockSaved->Apply();

    return S_OK;
}
