#pragma once
#include "Utils\DrawManager.h"
#include "SDK\Vector.h"

/// TODO LIST:
/// - Child tabs / menus
/// - Checkboxes
/// - Sliders
/// - Combo boxes


class MouseCursor
{
public:
    MouseCursor() { this->vecPointPos = g_Render.GetScreenCenter(); };
    void        Render();
    void        RunThink(UINT uMsg, LPARAM lParam);
    Vector2D    GetPosition() { return vecPointPos; };
    void        SetPosition(Vector2D vecPos) { this->vecPointPos = vecPos; };

    bool     bLMBPressed = false;
    bool     bRMBPressed = false;
    Vector2D vecPointPos;
};



class BaseWindow
{
public:
    BaseWindow(Vector2D vecPosition, Vector2D vecSize, CD3DFont* font, std::string strLabel = "");
    void SetWindowPos(Vector2D vecNewPosition) { this->vecWindowPos = vecNewPosition; };
    void UpdateData();
    void Render();
    int  GetHeaderHeight();
    
protected:
    std::string strLabel;       // Label / Name of the window
    CD3DFont*   pFont;          // Pointer to the font we will be using
    Vector2D vecWindowPos;      // Coordinates to top-left corner of the window
    Vector2D vecWindowSize;     // Size of the window
    int      iHeaderHeight;     // Header height in pixels
    bool     bIsDragged;
};


class MenuMain
{
public:
    MenuMain()
    {
        this->mainWindow = nullptr;
    };
    void Initialize();
    void RunThink(UINT uMsg, LPARAM lParam);
    void UpdateData();
    void Render();

private:
    std::unique_ptr<BaseWindow>  mainWindow;      // Pointer to our main window
};


