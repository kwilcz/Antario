#pragma once
// Thank you nvidia for ready-to-use solution :)
#include <d3dx9.h>
#include <d3d9.h>
#pragma comment (lib, "d3dx9")  // link D3DX DLL

// Releasing makro making sure we dont try to release a null pointer
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } }


// Font creation flags
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004

// Font rendering flags
#define D3DFONT_CENTERED_X  0x0001
#define D3DFONT_CENTERED_Y  0x0002
#define D3DFONT_TWOSIDED    0x0004
#define D3DFONT_FILTERED    0x0008
#define D3DFONT_SHADOW      0x0010




//-----------------------------------------------------------------------------
// Name: class CD3DFont
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class CD3DFont
{
    TCHAR   m_strFontName[80];            // Font properties
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;
    DWORD   m_dwFontWeight;

    LPDIRECT3DDEVICE9       m_pd3dDevice; // A D3DDevice used for rendering
    LPDIRECT3DTEXTURE9      m_pTexture;   // The d3d texture for this font
    LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer for rendering text
    DWORD   m_dwTexWidth;                 // Texture dimensions
    DWORD   m_dwTexHeight;
    FLOAT   m_fTextScale;
    FLOAT   m_fTexCoords[128 - 32][4];
    DWORD   m_dwSpacing;                  // Character pixel spacing per side

                                          // Stateblocks for setting and restoring render states
    LPDIRECT3DSTATEBLOCK9 m_pStateBlockSaved;
    LPDIRECT3DSTATEBLOCK9 m_pStateBlockDrawString;

public:
    // 2D and 3D text drawing functions
    HRESULT DrawString(FLOAT x, FLOAT y, DWORD dwColor,
            const char* strText, DWORD dwFlags = 0L);
    HRESULT DrawStringScaled(FLOAT x, FLOAT y, FLOAT fXScale, FLOAT fYScale, 
            DWORD dwColor, const TCHAR* strText, DWORD dwFlags = 0L);
    HRESULT Render3DText(FLOAT x, FLOAT y, const TCHAR* strText, DWORD dwFlags = 0L);

    // Function to get extent of text
    HRESULT GetTextExtent(const TCHAR* strText, SIZE* pSize);

    // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

    // Constructor / destructor
    CD3DFont(const TCHAR* strFontName, DWORD dwHeight, DWORD dwWeight, DWORD dwFlags = 0L);
    ~CD3DFont();
};