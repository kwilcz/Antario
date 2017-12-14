#include "DrawManager.h"
#include <assert.h>

// Init out global vars
Fonts       g_Fonts;
DrawManager g_Render;

struct Vertex
{
    float x, y, z, rhw;
    DWORD color;
};


DrawManager::DrawManager()
{
    this->pDevice         = nullptr;
    g_Fonts.pFontTahoma10 = nullptr;
}


void DrawManager::InitDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
{
    // Save the device internally
    this->pDevice = pDevice;

    // Get viewport for our center coords
    this->pDevice->GetViewport(&pViewPort);

    // Create new fonts
    g_Fonts.pFontTahoma8  = std::make_unique<CD3DFont>(L"Tahoma", 8,  FW_NORMAL);
    g_Fonts.pFontTahoma10 = std::make_unique<CD3DFont>(L"Tahoma", 10, FW_MEDIUM);

    // Init font device objects
    g_Fonts.InitDeviceObjects(pDevice);
}


void DrawManager::InvalidateDeviceObjects()
{
    // Remove a pointer to game device
    this->pDevice = nullptr;
    
    g_Fonts.InvalidateDeviceObjects();
}


void DrawManager::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
{
    this->pDevice = pDevice;
    this->pDevice->GetViewport(&pViewPort); 

    g_Fonts.InitDeviceObjects(pDevice);
}


void DrawManager::Line(Vector2D vecPos1, Vector2D vecPos2, Color color)
{
    this->Line(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, color);
}


void DrawManager::Line(float posx1, float posy1, float posx2, float posy2, Color color, bool antialiased)
{
    D3DCOLOR dwColor = COL2DWORD(color);
    Vertex vert[2] = 
    {  
        { posx1, posy1, 0.0f, 1.0f, dwColor },
        { posx2, posy2, 0.0f, 1.0f, dwColor } 
    };

    this->SetupRenderStates(antialiased);
    this->pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vert, sizeof(Vertex));
}


void DrawManager::Rect(Vector2D vecPos1, Vector2D vecPos2, Color color)
{
    this->Rect(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, color);
}


void DrawManager::Rect(float posx1, float posy1, float posx2, float posy2, Color color)
{
    this->Line(posx1, posy2, posx2, posy2, color, false); // draw top horizontal
    this->Line(posx1, posy1, posx2, posy1, color, false); // draw lower horizontal
    this->Line(posx1, posy1 + 1, posx1, posy2, color, false); // draw left vertical
    this->Line(posx2, posy1, posx2, posy2 + 1, color, false); // draw right vertical
}


void DrawManager::TriangleFilled(Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color)
{
    D3DCOLOR dwColor = COL2DWORD(color);
    Vertex vert[3] = 
    {  
        { pos1.x, pos1.y, 1.0f, 1.0f, dwColor },
        { pos2.x, pos2.y, 1.0f, 1.0f, dwColor },
        { pos3.x, pos3.y, 1.0f, 1.0f, dwColor } 
    };
    this->SetupRenderStates();
    this->pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, &vert, sizeof(Vertex));
}


void DrawManager::RectFilledGradient(Vector2D vecPos1, Vector2D vecPos2, Color col1, Color col2, GradientType vertical)
{
    D3DCOLOR dwColor  = COL2DWORD(col1);
    D3DCOLOR dwColor2 = COL2DWORD(col2);
    D3DCOLOR dwcol1, dwcol2, dwcol3, dwcol4;

    switch (vertical)
    {
        case GradientType::GRADIENT_VERTICAL:
            dwcol1 = dwColor;
            dwcol2 = dwColor;
            dwcol3 = dwColor2;
            dwcol4 = dwColor2;
            break;
        case GradientType::GRADIENT_HORIZONTAL:
            dwcol1 = dwColor;
            dwcol2 = dwColor2;
            dwcol3 = dwColor;
            dwcol4 = dwColor2;
            break;
    }

    Vertex vert[4] =
    {
        { vecPos1.x, vecPos1.y, 0.0f, 1.0f, dwcol1 },
        { vecPos2.x, vecPos1.y, 0.0f, 1.0f, dwcol2 },
        { vecPos1.x, vecPos2.y, 0.0f, 1.0f, dwcol3 },
        { vecPos2.x, vecPos2.y, 0.0f, 1.0f, dwcol4 }
    };

    this->SetupRenderStates();
    this->pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(Vertex));
}


void DrawManager::String(float posx, float posy, DWORD dwFlags, Color color, CD3DFont* pFont, const char* szText, ...)
{
    D3DCOLOR dwColor = COL2DWORD(color);
    pFont->DrawString(posx, posy, dwColor, szText, dwFlags);    // To make life easier
}


void DrawManager::SetupRenderStates(bool antialiased)
{
    this->pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    this->pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    this->pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    this->pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    this->pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    this->pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, antialiased);
}


Vector2D DrawManager::GetScreenCenter()
{
    return Vector2D((static_cast<float>(this->pViewPort.Width)*0.5f), (static_cast<float>(this->pViewPort.Height)*0.5f));
}
