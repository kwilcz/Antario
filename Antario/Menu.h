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

    bool bLMBPressed = false;
    bool bRMBPressed = false;
private:
    Vector2D vecPointPos;
};


class BaseWindow
{
public:
    BaseWindow(Vector2D vecPosition, Vector2D vecSize, std::string strLabel = "");
    void SetWindowPos(Vector2D vecNewPosition) { this->vecWindowPos = vecNewPosition; };
    void Render();

private:
    std::string strLabel;       // Label / Name of the window
    Vector2D vecWindowPos;      // Coordinates to top-left corner of the window
};


class Menu
{
public:
    Menu()
    {
        this->mainWindow = nullptr;
        this->mouseCursor = nullptr;
    };
    void Initialize();
    void RunThink(UINT uMsg, LPARAM lParam);
    void Render();
private:
    std::unique_ptr<BaseWindow>  mainWindow;
    std::unique_ptr<MouseCursor> mouseCursor; // Pointer to our mouse cursor
};
