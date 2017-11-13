#include "DrawManager.h"

struct Vertex
{
    float x, y, z, rhw;
    DWORD color;
};

DrawManager::DrawManager()
{
    this->pDevice   = nullptr;
    this->pFont     = nullptr;
}


void DrawManager::Init(LPDIRECT3DDEVICE9 pDevice)
{
    // Save the device internally
    this->pDevice = pDevice;

    // Create a new font
    D3DXCreateFontA(this->pDevice, 12, 0, FW_NORMAL, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Tahoma", &this->pFont);
}


void DrawManager::Invalidate()
{
    if (this->pDevice)
        this->pDevice->Release();
    
    this->pFont->Release();
}


void DrawManager::Reset(LPDIRECT3DDEVICE9 pDevice)
{
    this->Invalidate();
    this->Init(pDevice);
}


void DrawManager::DrawLine(int x, int y, int x2, int y2, DWORD color)
{
    Vertex vert[2] = {  { static_cast<float>(x),  static_cast<float>(y),  0.0f, 1.0f, color },
                        { static_cast<float>(x2), static_cast<float>(y2), 0.0f, 1.0f, color } };

    this->pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    this->pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vert, sizeof(Vertex));
}

/// TODO: Test this, wrote that from my mem.
void DrawManager::DrawRect(int x, int y, int width, int height, DWORD color)
{
    this->DrawLine(x, y, x + width, y, color);  // draw lower horizontal
    this->DrawLine(x, y, x, y + height, color); // draw left vertical
    this->DrawLine(x, y + height - 1, x + width, y + height - 1, color);    // draw top horizontal
    this->DrawLine(x + width - 1, y, x + width - 1, y + height, color);     // draw right vertical
}
