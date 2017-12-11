#include "Menu.h"
#include "Settings.h"

#include "SDK\IVEngineClient.h"

// Defined to avoid including windowsx.h
#define GET_X_LPARAM(lp)                        ((float)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((float)(short)HIWORD(lp))


std::unique_ptr<MouseCursor> g_MouseCursor;   // Pointer to our mouse cursor


void MouseCursor::Render()
{
    float x = this->vecPointPos.x;
    float y = this->vecPointPos.y;

    // Draw inner fill color
    Vector2D vecPos1 = Vector2D(x + 1, y + 1);
    Vector2D vecPos2 = Vector2D(x + 25, y + 12);
    Vector2D vecPos3 = Vector2D(x + 12, y + 25);
    g_Render.TriangleFilled(vecPos1, vecPos2, vecPos3, g_Settings.colCursor);

    // Draw second smaller inner fill color
    vecPos1 = Vector2D(x + 6, y + 6);
    vecPos2 = Vector2D(x + 19, y + 12);
    vecPos3 = Vector2D(x + 12, y + 19);
    g_Render.TriangleFilled(vecPos1, vecPos2, vecPos3, g_Settings.colCursor);

    // Draw border
    g_Render.Line(x, y, x + 25, y + 12, Color(0, 0, 0, 200));
    g_Render.Line(x, y, x + 12, y + 25, Color(0, 0, 0, 200));
    g_Render.Line(x + 25, y + 12, x + 12, y + 25, Color(0, 0, 0, 200));
}


void MouseCursor::RunThink(UINT uMsg, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
        this->SetPosition(Vector2D(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
        break;
    case WM_LBUTTONDOWN:
        this->bLMBPressed = true;
        break;
    case WM_LBUTTONUP:
        this->bLMBPressed = false;
        break;
    case WM_RBUTTONDOWN:
        this->bRMBPressed = true;
        break;
    case WM_RBUTTONUP:
        this->bRMBPressed = false;
        break;
    }
}


BaseWindow::BaseWindow(Vector2D vecPosition, Vector2D vecSize, CD3DFont* pFont, std::string strLabel)
{
    this->pFont = pFont;
    this->strLabel = strLabel;
    this->vecWindowSize = vecSize;
    SetWindowPos(vecPosition);
    this->iHeaderHeight = this->GetHeaderHeight();
}


void BaseWindow::UpdateData()
{
    // Lambda checking if our mouse cursor is in bounds of the selectable. Will be moved later on
    auto IsInBounds = [this](Vector2D vecSrc, Vector2D vecDst1, Vector2D vecDst2) -> bool
    {
        if (vecSrc.x > vecDst1.x && vecSrc.x < vecDst2.x && vecSrc.y > vecDst1.y && vecSrc.y < vecDst2.y)
            return true;
        else
            return false;
    };

    // Check if mouse has been pressed in the proper area. If yes, save window state as dragged.
    Vector2D vecHeaderBounds = Vector2D(this->vecWindowPos.x + this->vecWindowSize.x, this->vecWindowPos.y + this->iHeaderHeight);
    if (g_MouseCursor->bLMBPressed && IsInBounds(g_MouseCursor->vecPointPos, this->vecWindowPos, vecHeaderBounds))
    {
        this->bIsDragged = true;
    }
    else
    if (!g_MouseCursor->bLMBPressed)
        this->bIsDragged = false;

    // Check if the window is dragged. If it is, move window by the cursor difference between ticks.
    static Vector2D vecOldMousePos = g_MouseCursor->vecPointPos;
    if (this->bIsDragged)
    {
        Vector2D vecNewPos = this->vecWindowPos + (g_MouseCursor->vecPointPos - vecOldMousePos);
        this->SetWindowPos(vecNewPos);
        vecOldMousePos = g_MouseCursor->vecPointPos;
    }
    else
        vecOldMousePos = g_MouseCursor->vecPointPos;

}

void BaseWindow::Render()
{
    // Draw main background rectangle
    g_Render.RectFilledGradient(this->vecWindowPos, this->vecWindowPos + this->vecWindowSize, Color(50, 50, 50, 200), Color(20, 20, 20, 200), GradientType::GRADIENT_VERTICAL);
    // Draw header rect.
    g_Render.RectFilledGradient(this->vecWindowPos, Vector2D(this->vecWindowPos.x + this->vecWindowSize.x, this->vecWindowPos.y + this->iHeaderHeight), Color(50, 50, 50, 200), Color(20, 20, 20, 200), GradientType::GRADIENT_VERTICAL);

    // Draw title string, defined as label.
    g_Render.String(this->vecWindowPos.x +(this->vecWindowSize.x / 2.f), this->vecWindowPos.y, D3DFONT_CENTERED_X, Color(200, 200, 200), g_Fonts.pFontTahoma10.get(), this->strLabel.c_str());
}

int BaseWindow::GetHeaderHeight()
{
    SIZE size;
    this->pFont->GetTextExtent(this->strLabel.c_str(), &size);
    return size.cy + 1;
}


void MenuMain::Initialize()
{
    g_MouseCursor = std::make_unique<MouseCursor>();    // Create our mouse cursor
    this->mainWindow  = std::make_unique<BaseWindow>(Vector2D(450, 450), Vector2D(360, 256), g_Fonts.pFontTahoma10.get(), "Antario - Main"); // Create our main window (Could have multiple if you'd create vec. for it)
    {
        // define future child windows etc.
    }
}


void MenuMain::Render()
{  
    this->mainWindow->Render();     // Render main window
    g_MouseCursor->Render();    // Render mouse cursor in the end  
}

void MenuMain::RunThink(UINT uMsg, LPARAM lParam)
{
    g_MouseCursor->RunThink(uMsg, lParam);
/// TODO: Capture keyboard input
}

void MenuMain::UpdateData()
{
    this->mainWindow->UpdateData();
}
