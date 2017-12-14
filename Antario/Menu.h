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

    // Set&get actual mouse position
    Vector2D            GetPosition() { return vecPointPos; };
    virtual void        SetPosition(Vector2D vecPos) { this->vecPointPos = vecPos; };

    // Checks if the mouse is in specified bounds
    virtual bool        IsInBounds(Vector2D vecDst1, Vector2D vecDst2);

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

    // Size and position get/set
    virtual Vector2D GetPos()                        { return this->vecPosition; };
    virtual void     SetPos(Vector2D vecNewPosition) { this->vecPosition = vecNewPosition; };
    virtual Vector2D GetSize()                       { return this->vecSize; };
    virtual void     SetSize(Vector2D vecNewSize)    { this->vecSize - vecNewSize; };
    
    // Parent/child setting functions
    virtual void SetParent(MenuMain* parent);
    virtual void AddChild(std::shared_ptr<MenuMain> child);
        
    static std::unique_ptr<MouseCursor> mouseCursor;  // Pointer to our mouse cursor
    static CD3DFont*   pFont;                         // Pointer to the font we will be using
protected:
    MenuMain*   pParent;
    std::vector<std::shared_ptr<MenuMain>> vecChildren;
    std::string strLabel;       // Label / Name of the window
    Vector2D    vecPosition;    // Coordinates to top-left corner of the drawed ent.
    Vector2D    vecSize;        // Size of the drawed ent.
};


class BaseWindow : public MenuMain
{
public:
    BaseWindow(Vector2D vecPosition, Vector2D vecSize, CD3DFont* font, CD3DFont* headerFont, std::string strLabel = "");
    virtual void Render();
    virtual void UpdateData();

    virtual int  SetHeaderHeight();
private:
    CD3DFont* pHeaderFont;      // Header only font
    int       iHeaderHeight;    // Header height in pixels
    bool      bIsDragged;       // Defines if the window is currently dragged
};


class Checkbox : public MenuMain
{
public:
    Checkbox(std::string strLabel, bool *bValue);
    virtual void Render();
    virtual void UpdateData();
private:
    Vector2D vecSelectableSize;
    bool*    bCheckboxValue;
};
