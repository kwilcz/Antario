#include "DrawManager.h"
#include <assert.h>

DrawManager g_Render;

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

    pFont = std::make_unique<CD3DFont>(L"Tahoma", 12, FW_BOLD);

    // Init font device object
    pFont->InitDeviceObjects(pDevice);
    pFont->RestoreDeviceObjects();
}


void DrawManager::Invalidate()
{
    // No need for checks as SAFE_RELEASE does that for us
    SAFE_RELEASE(this->pDevice);

    this->pFont->InvalidateDeviceObjects();
}


void DrawManager::Reset(LPDIRECT3DDEVICE9 pDevice)
{
    this->pDevice = pDevice;

    pFont->InvalidateDeviceObjects();
    pFont->InitDeviceObjects(pDevice);
    pFont->RestoreDeviceObjects();
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

void DrawManager::DrawString(float x, float y, DWORD dwColor, const char * szText, DWORD dwFlags)
{
    pFont->DrawString(x, y, dwColor, szText, dwFlags);    // To make life easier
}
