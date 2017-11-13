#pragma once
#include <d3dx9.h>
#pragma comment (lib, "d3dx9")  // link D3DX DLL

class DrawManager
{
public: // Function members
    // Basic non-drawing functions
    DrawManager();

    void Init       (LPDIRECT3DDEVICE9 pDevice);
    void Reset      (LPDIRECT3DDEVICE9 pDevice);
    void Invalidate ();

    // Drawing functions

    void DrawLine   (int x, int y, int x2, int y2, DWORD color);
    void DrawRect   (int x, int y, int width, int height, DWORD color);

private: // Variable members
    LPDIRECT3DDEVICE9 pDevice;
    LPD3DXFONT pFont;
};
