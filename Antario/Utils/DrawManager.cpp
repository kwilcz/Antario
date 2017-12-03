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


void DrawManager::DrawLine(Vector2D vecPos1, Vector2D vecPos2, DWORD dwColor)
{
    this->DrawLine(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, dwColor);
}


void DrawManager::DrawLine(float x, float y, float x2, float y2, DWORD dwColor)
{
    Vertex vert[2] = {  { x,  y,  0.0f, 1.0f, dwColor },
                        { x2, y2, 0.0f, 1.0f, dwColor } };

    this->pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
    this->pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vert, sizeof(Vertex));
}


void DrawManager::DrawRect(Vector2D vecPos1, Vector2D vecPos2, DWORD dwColor)
{
    this->DrawRect(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, dwColor);
}


/// TODO: Test this, wrote that from my mem.
void DrawManager::DrawRect(float x, float y, float width, float height, DWORD dwColor)
{
    this->DrawLine(x, y, x + width, y, dwColor);  // draw lower horizontal
    this->DrawLine(x, y, x, y + height, dwColor); // draw left vertical
    this->DrawLine(x, y + height - 1, x + width, y + height - 1, dwColor);    // draw top horizontal
    this->DrawLine(x + width - 1, y, x + width - 1, y + height, dwColor);     // draw right vertical
}

void DrawManager::DrawString(float x, float y, DWORD dwFlags, DWORD dwColor, const char* szText, ...)
{
    pFont->DrawString(x, y, dwColor, szText, dwFlags);    // To make life easier
}
