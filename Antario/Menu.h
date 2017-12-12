#pragma once
#include <vector>
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
    virtual void        Render();
    virtual void        RunThink(UINT uMsg, LPARAM lParam);
    Vector2D    GetPosition() { return vecPointPos; };
    virtual void        SetPosition(Vector2D vecPos) { this->vecPointPos = vecPos; };

    bool     bLMBPressed = false;
    bool     bRMBPressed = false;
    Vector2D vecPointPos;
};


class MenuMain
{
public:
    MenuMain() {};
    virtual void RunThink(UINT uMsg, LPARAM lParam);
    virtual void Initialize();
    virtual void UpdateData();
    virtual void Render();
    
    // Parent/child setting functions
    virtual void SetParent(MenuMain* parent);
    virtual void AddChild(std::shared_ptr<MenuMain> child);

    // Static pointer to our mouse cursor, so we have only one copy of it
    static std::unique_ptr<MouseCursor> mouseCursor;   
protected:
    MenuMain*   pParent;
    std::vector<std::shared_ptr<MenuMain>> vecChildren;
    std::string strLabel;       // Label / Name of the window
    CD3DFont*   pFont;          // Pointer to the font we will be using
    Vector2D vecWindowPos;      // Coordinates to top-left corner of the window
    Vector2D vecWindowSize;     // Size of the window
    int      iHeaderHeight;     // Header height in pixels
};

class BaseWindow : public MenuMain
{
public:
    BaseWindow(Vector2D vecPosition, Vector2D vecSize, CD3DFont* font, std::string strLabel = "");
    virtual void Render();
    virtual void UpdateData();

    virtual int  SetHeaderHeight();
    virtual void SetWindowPos(Vector2D vecNewPosition) { this->vecWindowPos = vecNewPosition; };
private:
    bool     bIsDragged;
};




