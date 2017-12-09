#include "Menu.h"
#include "Settings.h"

#include "SDK\IVEngineClient.h"

// Defined to avoid including windowsx.h
#define GET_X_LPARAM(lp)                        ((float)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((float)(short)HIWORD(lp))


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


BaseWindow::BaseWindow(Vector2D vecPosition, Vector2D vecSize, std::string strLabel)
{
    this->strLabel = strLabel;
    SetWindowPos(vecPosition);
}


void BaseWindow::Render()
{
/// TODO: Render the main window
}


void Menu::Initialize()
{
    this->mouseCursor = std::make_unique<MouseCursor>();    // Create our mouse cursor
    this->mainWindow = std::make_unique<BaseWindow>(Vector2D(450, 450), Vector2D(360, 256), "Antario - Main"); // Create our main window (Could have multiple if you'd define it in vec.
    {
        // define future child windows etc.
    }
}


void Menu::Render()
{
    this->mouseCursor->Render();    // Render mouse cursor    
    this->mainWindow->Render();     // Render main window
}

void Menu::RunThink(UINT uMsg, LPARAM lParam)
{
    this->mouseCursor->RunThink(uMsg, lParam);
/// TODO: Capture keyboard input
}