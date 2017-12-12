#include "Menu.h"
#include "Settings.h"

#include "SDK\IVEngineClient.h"

// Defined to avoid including windowsx.h
#define GET_X_LPARAM(lp)                        ((float)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((float)(short)HIWORD(lp))

// Init our static pointer
std::unique_ptr<MouseCursor> MenuMain::mouseCursor;

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
    this->iHeaderHeight = this->SetHeaderHeight();
    this->SetWindowPos(vecPosition);
}


void BaseWindow::Render()
{
    // Draw main background rectangle
    g_Render.RectFilledGradient(this->vecWindowPos, this->vecWindowPos + this->vecWindowSize, Color(50, 50, 50, 200), Color(20, 20, 20, 200), GradientType::GRADIENT_VERTICAL);
    // Draw header rect.
    g_Render.RectFilledGradient(this->vecWindowPos, Vector2D(this->vecWindowPos.x + this->vecWindowSize.x, this->vecWindowPos.y + this->iHeaderHeight), Color(50, 50, 50, 200), Color(20, 20, 20, 200), GradientType::GRADIENT_VERTICAL);

    // Draw title string, defined as label.
    g_Render.String(this->vecWindowPos.x +(this->vecWindowSize.x / 2.f), this->vecWindowPos.y, D3DFONT_CENTERED_X, Color(200, 200, 200), this->pFont, this->strLabel.c_str());
    
    // Render all childrens
    MenuMain::Render();
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
    if (this->mouseCursor->bLMBPressed && IsInBounds(this->mouseCursor->vecPointPos, this->vecWindowPos, vecHeaderBounds))
        this->bIsDragged = true;
    else
    if (!this->mouseCursor->bLMBPressed)
        this->bIsDragged = false;

    // Check if the window is dragged. If it is, move window by the cursor difference between ticks.
    static Vector2D vecOldMousePos = this->mouseCursor->vecPointPos;
    if (this->bIsDragged)
    {
        Vector2D vecNewPos = this->vecWindowPos + (this->mouseCursor->vecPointPos - vecOldMousePos);
        this->SetWindowPos(vecNewPos);
        vecOldMousePos = this->mouseCursor->vecPointPos;
    }
    else
        vecOldMousePos = this->mouseCursor->vecPointPos;

    MenuMain::UpdateData();
}


int BaseWindow::SetHeaderHeight()
{
    SIZE size;
    this->pFont->GetTextExtent(this->strLabel.c_str(), &size);
    return size.cy + 1;
}


void MenuMain::SetParent(MenuMain* newParent)
{
    this->pParent = newParent;
}


void MenuMain::AddChild(std::shared_ptr<MenuMain> child)
{
    child->SetParent(this);
    this->vecChildren.push_back(child);
}


void MenuMain::Render()
{    
    if (this->vecChildren.size() > 0)
    {
        // Render all childrens
        for (auto it = this->vecChildren.begin(); it != this->vecChildren.end(); it++)
            (*it)->Render();
    }
}


void MenuMain::RunThink(UINT uMsg, LPARAM lParam)
{
    this->mouseCursor->RunThink(uMsg, lParam);
/// TODO: Capture keyboard input
}


void MenuMain::Initialize()
{
    std::shared_ptr<BaseWindow> mainWindow = std::make_shared<BaseWindow>(Vector2D(450, 450), Vector2D(360, 256), g_Fonts.pFontTahoma10.get(), "Antario - Main"); // Create our main window (Could have multiple if you'd create vec. for it)
    {
        // All child menus / buttons etc, will be done in the future.
    }
    this->AddChild(mainWindow);

    this->mouseCursor = std::make_unique<MouseCursor>();    // Create our mouse cursor (one instance only)
}

void MenuMain::UpdateData()
{
    if (this->vecChildren.size() > 0)
    {
        for (auto it = this->vecChildren.begin(); it != this->vecChildren.end(); it++)
            (*it)->UpdateData();
    }
}
