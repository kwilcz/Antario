#pragma once
#include <vector>
#include "Utils\DrawManager.h"
#include "SDK\Vector.h"

/// TODO LIST:
/// - Sliders
/// - Combo boxes


struct MenuStyle
{
    int   iPaddingX      = 20;                         /*- Padding between sections -*/
    int   iPaddingY      = 10;                         /*- Padding between selectables -*/
    Color colSectionOutl = {0, 0, 0, 100};             /*- Color of the section outline -*/
    Color colCheckbox1   = {50, 50, 50, 255};          /*- Color of the first gradient color of the checkbox -*/
    Color colCheckbox2   = {35, 35, 35, 255};          /*- Color of the second gradient color of the checkbox -*/
    Color colText        = {160, 160, 160, 255};       /*- Color of the text inside the main window -*/
    Color colHeaderText  = {200, 200, 215};            /*- Color of the text inside the header strip -*/
};


class MouseCursor
{
public:
         MouseCursor() { this->vecPointPos = g_Render.GetScreenCenter(); };
    void Render();
    void RunThink(UINT uMsg, LPARAM lParam);

    // Set&get actual mouse position
    Vector2D GetPosition() { return vecPointPos; };
    void     SetPosition(Vector2D vecPos) { this->vecPointPos = vecPos; };

    // Checks if the mouse is in specified bounds
    bool IsInBounds(Vector2D vecDst1, Vector2D vecDst2);

    bool     bLMBPressed = false;
    bool     bRMBPressed = false;
    Vector2D vecPointPos;
};


class MenuMain
{
public:
    MenuMain(): pParent(nullptr) {} ;
    virtual ~MenuMain() = default;
    virtual void RunThink(UINT uMsg, LPARAM lParam);
    virtual void Initialize();
    virtual void UpdateData();
    virtual void Render();

    // Size and position get/set
    virtual Vector2D GetPos()                               { return this->vecPosition; };
    virtual void     SetPos(const Vector2D& vecNewPosition) { this->vecPosition = vecNewPosition; };
    virtual Vector2D GetSize()                              { return this->vecSize; };
    virtual void     SetSize(const Vector2D& vecNewSize)    { this->vecSize     = vecNewSize; };
    
    // Parent/child setting functions
    virtual void SetParent(MenuMain* newParent);
    virtual void AddChild(const std::shared_ptr<MenuMain>& child);

    static MenuStyle                    style;
    static std::unique_ptr<MouseCursor> mouseCursor;  // Pointer to our mouse cursor
    static CD3DFont*                    pFont;                         // Pointer to the font we will be using
protected:
    MenuMain*                              pParent;
    std::vector<std::shared_ptr<MenuMain>> vecChildren;
    std::string                            strLabel;       // Label / Name of the window
    Vector2D                               vecPosition;    // Coordinates to top-left corner of the drawed ent.
    Vector2D                               vecSize;        // Size of the drawed ent.
};


class BaseWindow : public MenuMain
{
public:
    BaseWindow(): pHeaderFont(nullptr), iHeaderHeight(0) {} ;
    BaseWindow(Vector2D             vecPosition, Vector2D vecSize, CD3DFont* pFont, CD3DFont* pHeaderFont,
                        std::string strLabel = "");
    virtual void Render();
    virtual void UpdateData();

    static bool bIsDragged;// Defines if the window is currently dragged
private:
    virtual int GetHeaderHeight();

    CD3DFont* pHeaderFont;      // Header only font
    int       iHeaderHeight;    // Header height in pixels
};


class BaseSection : public BaseWindow
{
public:
                 BaseSection(Vector2D vecSize, int iNumRows);
    virtual void Render();
    virtual void UpdateData();

private:
    virtual void SetupPositions();
    bool         bIsInitialized = false;
    int          iNumRows;
};


class Checkbox : public MenuMain
{
public:
                 Checkbox(std::string strLabel, bool* bValue);
    virtual void Render();
    virtual void UpdateData();
private:
    Vector2D vecSelectableSize;
    Vector2D vecSelectablePosition;
    bool     bIsHovered;
    bool*    bCheckboxValue;
};


class Button : public MenuMain
{
public:
                 Button(std::string strLabel, void (&fnPointer)());
    virtual void Render();
    virtual void UpdateData();
private:
    void (*fnActionPlay)();
    bool   bIsHovered;
};
