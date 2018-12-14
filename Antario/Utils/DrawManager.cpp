#include "DrawManager.h"

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
    this->pDevice = nullptr;
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


void DrawManager::Line(SPoint vecPos1, SPoint vecPos2, Color color) const
{
    this->Line(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, color);
}


void DrawManager::Line(int posx1, int posy1, int posx2, int posy2, Color color) const
{
    D3DCOLOR dwColor = COL2DWORD(color);
    Vertex vert[2] =
    {
        { float(posx1), float(posy1), 0.0f, 1.0f, dwColor },
        { float(posx2), float(posy2), 0.0f, 1.0f, dwColor }
    };

    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vert, sizeof(Vertex));
}


void DrawManager::Rect(SRect rcBouds, Color color) const
{
    this->Rect(rcBouds.left, rcBouds.top, rcBouds.right, rcBouds.bottom, color);
}


void DrawManager::Rect(SPoint vecPos1, SPoint vecPos2, Color color) const
{
    this->Rect(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, color);
}


void DrawManager::Rect(int posx1, int posy1, int posx2, int posy2, Color color) const
{
    D3DCOLOR dwColor = COL2DWORD(color);

    /* Fix that fuckin offset of the rectangles */
    posx2 -= 1; posy2 -= 1;

    Vertex vert[5] =
    {   // Draw lines between declared points, needs primitive count as number of lines (4 here)
        { float(posx1), float(posy1), 1.0f, 1.0f, dwColor }, // Top left corner
        { float(posx2), float(posy1), 1.0f, 1.0f, dwColor }, // Top right corner
        { float(posx2), float(posy2), 1.0f, 1.0f, dwColor }, // Bottom right corner
        { float(posx1), float(posy2), 1.0f, 1.0f, dwColor }, // Bottom left corner
        { float(posx1), float(posy1), 1.0f, 1.0f, dwColor }  // Back to top left to finish drawing
    };

    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);  /* Disabled, cause corners of the rectangle get antialiased */
    this->pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, &vert, sizeof(Vertex));
    this->pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);   /* And enable again for the rest of the drawing */
}

void DrawManager::RectBordered(SRect rcBounds, Color color) const
{
    this->RectBordered(rcBounds.left, rcBounds.top, rcBounds.right, rcBounds.bottom, color);
}


void DrawManager::RectBordered(SPoint vecPos1, SPoint vecPos2, Color color) const
{
    this->RectBordered(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, color);
}


void DrawManager::RectBordered(int posx1, int posy1, int posx2, int posy2, Color color) const
{
    auto col  = Color::Black();
    col.alpha = color.alpha;
    this->Rect(posx1, posy1, posx2, posy2, color);
    this->Rect(posx1 - 1, posy1 - 1, posx2 + 1, posy2 + 1, col);
    this->Rect(posx1 + 1, posy1 + 1, posx2 - 1, posy2 - 1, col);
}

void DrawManager::RectFilled(SRect rcPosition, Color color) const
{
    this->RectFilled(rcPosition.left, rcPosition.top, rcPosition.right, rcPosition.bottom, color);
}


void DrawManager::RectFilled(SPoint vecPos1, SPoint vecPos2, Color color) const
{
    this->RectFilled(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, color);
}

void DrawManager::RectFilled(int posx1, int posy1, int posx2, int posy2, Color color) const
{
    D3DCOLOR dwColor = COL2DWORD(color);

    Vertex vert[4] =
    {
        { float(posx1), float(posy1), 0.0f, 1.0f, dwColor },
        { float(posx2), float(posy1), 0.0f, 1.0f, dwColor },
        { float(posx1), float(posy2), 0.0f, 1.0f, dwColor },
        { float(posx2), float(posy2), 0.0f, 1.0f, dwColor }
    };
        
    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(Vertex));
}

void DrawManager::Triangle(SPoint pos1, SPoint pos2, SPoint pos3, Color color) const
{
    D3DCOLOR dwColor = COL2DWORD(color);
    Vertex vert[4] =
    {
        { float(pos1.x), float(pos1.y), 0.0f, 1.0f, dwColor },
        { float(pos2.x), float(pos2.y), 0.0f, 1.0f, dwColor },
        { float(pos3.x), float(pos3.y), 0.0f, 1.0f, dwColor },
        { float(pos1.x), float(pos1.y), 0.0f, 1.0f, dwColor }
    };

    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, &vert, sizeof(Vertex));
}


void DrawManager::TriangleFilled(SPoint pos1, SPoint pos2, SPoint pos3, Color color) const
{
    D3DCOLOR dwColor = COL2DWORD(color);
    Vertex vert[3] =
    {
        { float(pos1.x), float(pos1.y), 0.0f, 1.0f, dwColor },
        { float(pos2.x), float(pos2.y), 0.0f, 1.0f, dwColor },
        { float(pos3.x), float(pos3.y), 0.0f, 1.0f, dwColor }
    };

    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, &vert, sizeof(Vertex));
}


void DrawManager::RectFilledGradient(SRect rcBoudingRect, Color col1, Color col2, GradientType type) const
{
    this->RectFilledGradient(rcBoudingRect.left, rcBoudingRect.top, rcBoudingRect.right, rcBoudingRect.bottom, col1, col2, type);
}

void DrawManager::RectFilledGradient(SPoint vecPos1, SPoint vecPos2, Color col1, Color col2, GradientType vertical) const
{
    this->RectFilledGradient(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, col1, col2, vertical);
}

void DrawManager::RectFilledGradient(int posx1, int posy1, int posx2, int posy2, Color col1, Color col2, GradientType vertical) const
{
    const auto dwColor  = COL2DWORD(col1);
    const auto dwColor2 = COL2DWORD(col2);
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
    default:
        dwcol1 = D3DCOLOR_RGBA(255, 255, 255, 255);
        dwcol4 = dwcol3 = dwcol2 = dwcol1;
    }

    Vertex vert[4] =
    {
        { float(posx1), float(posy1), 0.0f, 1.0f, dwcol1 },
        { float(posx2), float(posy1), 0.0f, 1.0f, dwcol2 },
        { float(posx1), float(posy2), 0.0f, 1.0f, dwcol3 },
        { float(posx2), float(posy2), 0.0f, 1.0f, dwcol4 }
    };
    
    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(Vertex));
}

void DrawManager::RectFilledGradientMultiColor(SRect rcBoudingRect, Color colTopLeft, Color colTopRight, Color colBottomLeft, Color colBottomRight) const
{
    this->RectFilledGradientMultiColor(rcBoudingRect.left, rcBoudingRect.top, rcBoudingRect.right, rcBoudingRect.bottom, colTopLeft, colTopRight,
        colBottomLeft, colBottomRight);
}


void DrawManager::RectFilledGradientMultiColor(SPoint vecPos1, SPoint vecPos2, Color colTopLeft, Color colTopRight, Color colBottomLeft, Color colBottomRight) const
{
    this->RectFilledGradientMultiColor(vecPos1.x, vecPos1.y, vecPos2.x, vecPos2.y, colTopLeft, colTopRight, colBottomLeft, colBottomRight);
}


void DrawManager::RectFilledGradientMultiColor(int posx1, int posy1, int posx2, int posy2, Color colTopLeft, Color colTopRight, Color colBottomLeft, Color colBottomRight) const
{
    Vertex vert[4] =
    {
        { float(posx1), float(posy1), 0.0f, 1.0f, COL2DWORD(colTopLeft) },
        { float(posx2), float(posy1), 0.0f, 1.0f, COL2DWORD(colTopRight) },
        { float(posx1), float(posy2), 0.0f, 1.0f, COL2DWORD(colBottomLeft) },
        { float(posx2), float(posy2), 0.0f, 1.0f, COL2DWORD(colBottomRight) }
    };
        
    this->pDevice->SetTexture(0, nullptr);
    this->pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vert, sizeof(Vertex));
}


void DrawManager::String(SPoint vecPos, DWORD dwFlags, Color color, CD3DFont* pFont, const char* szText) const
{
    pFont->DrawString(vecPos.x, vecPos.y, COL2DWORD(color), szText, dwFlags);
}


void DrawManager::String(int posx, int posy, DWORD dwFlags, Color color, CD3DFont* pFont, const char* szText) const
{
    pFont->DrawString(FLOAT(posx), FLOAT(posy), COL2DWORD(color), szText, dwFlags);
}


/*
*   SetupRenderStates - Sets RenderStates for our custom StateBlock
*   It's required to draw everything independent on game settings.
*/
void DrawManager::SetupRenderStates() const
{
    this->pDevice->SetVertexShader(nullptr);
    this->pDevice->SetPixelShader(nullptr);
    this->pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    this->pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    this->pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    this->pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    this->pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    this->pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    this->pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    this->pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    this->pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

    this->pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
    this->pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

    this->pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    this->pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    this->pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    this->pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    this->pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
    this->pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    this->pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

    this->pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
    this->pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
                                                          D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}


SPoint DrawManager::GetScreenCenter() const
{
    return SPoint(static_cast<int>(static_cast<int>(this->pViewPort.Width) * 0.5f), static_cast<int>(static_cast<int>(this->pViewPort.Height) * 0.5f));
}


void DrawManager::SetCustomViewport(const D3DVIEWPORT9& pNewViewport)
{
    this->pDevice->SetViewport(&pNewViewport);
}

void DrawManager::SetCustomViewport(const SRect& vpRect)
{
    auto newVP = D3DVIEWPORT9{ DWORD(vpRect.left), DWORD(vpRect.top), DWORD(vpRect.Width()), DWORD(vpRect.Height()), 0, 0 };
    this->SetCustomViewport(newVP);
}

void DrawManager::RestoreOriginalViewport()
{
    this->pDevice->SetViewport(&this->pViewPort);
}

void DrawManager::SetCustomScissorRect(const SRect& rcRect)
{
    RECT rc{};
    pScissorRect.push(rc);
    this->pDevice->GetScissorRect(&pScissorRect.back());
    this->pDevice->SetScissorRect(reinterpret_cast<const RECT*>(&rcRect));
}

void DrawManager::RestoreOriginalScissorRect()
{
    this->pDevice->SetScissorRect(&pScissorRect.back());
    pScissorRect.pop();
}