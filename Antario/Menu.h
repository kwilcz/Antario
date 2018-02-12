#pragma once
#include <vector>
#include "Utils\DrawManager.h"
#include "SDK\Vector.h"

/// TODO LIST:
/// - Sliders
enum class MenuSelectableType;


struct MenuStyle
{
    int   iPaddingX       = 20;                     /*- Padding between sections                            -*/
    int   iPaddingY       = 6;                      /*- Padding between selectables                         -*/
    Color colHover        = { 100, 100, 100, 50 };  /*- Color applied on the obj when mouse hovers above it -*/
    Color colSectionOutl  = { 15, 15, 15, 200 };    /*- Color of the section outline                        -*/
    Color colSectionFill  = { 0, 0, 0, 25 };        /*- Color filling the section bounds                    -*/
    Color colCheckbox1    = { 50, 50, 50, 255 };    /*- Color of the first gradient color of the checkbox   -*/
    Color colCheckbox2    = { 35, 35, 35, 255 };    /*- Color of the second gradient color of the checkbox  -*/
    Color colText         = { 160, 160, 160, 255 }; /*- Color of the text inside the main window            -*/
    Color colHeaderText   = { 200, 200, 215 };      /*- Color of the text inside the header strip           -*/
    Color colComboBoxRect = { 50, 50, 50, 180 };    /*- Color of the combobox rectangle                     -*/
};



class MouseCursor
{
public:
    MouseCursor() { this->vecPointPos = g_Render.GetScreenCenter(); };
    virtual      ~MouseCursor() = default;
    virtual void Render();
    virtual void RunThink(UINT uMsg, LPARAM lParam);

    Vector2D     GetPosition() const { return vecPointPos; }; /* Set&get actual mouse position */
    virtual void SetPosition(Vector2D vecPos) { this->vecPointPos = vecPos; };

    /* Checks if the mouse is in specified bounds */
    virtual bool IsInBounds(Vector2D vecDst1, Vector2D vecDst2);

    bool     bLMBPressed = false; /* Actual state of Left Mouse Button (pressed or not)   */
    bool     bRMBPressed = false; /* Actual state of Right Mouse Button (pressed or not)  */
    Vector2D vecPointPos;         /* Current mouse cursor position                        */
};



class MenuMain
{
public:
    MenuMain() : pParent(nullptr), flMaxChildWidth(0) {};
    virtual      ~MenuMain() = default;
    virtual void RunThink(UINT uMsg, LPARAM lParam);
    virtual void Initialize();
    virtual bool UpdateData();      /* Updates all the needed data like size, position, etc. */
    virtual void Render();          /* Renders the object                                    */

                                    /* Size and position get/set     */
    virtual Vector2D GetPos() { return this->vecPosition; };
    virtual void     SetPos(Vector2D vecNewPosition) { this->vecPosition = vecNewPosition; };
    virtual Vector2D GetSize() { return this->vecSize; };
    virtual void     SetSize(Vector2D vecNewSize) { this->vecSize - vecNewSize; };
    virtual float    GetMaxChildWidth() { return this->flMaxChildWidth; };

    /* Parent/child setting functions */
    virtual void SetParent(MenuMain* parent);
    virtual void AddChild(const std::shared_ptr<MenuMain>& child);

    static std::unique_ptr<MouseCursor> mouseCursor; /* Pointer to our mouse cursor                             */
    static MenuStyle style;                          /* Struct containing all colors / paddings in our menu.    */
    static CD3DFont* pFont;                          /* Pointer to the font used in the menu.                   */

    std::vector<std::shared_ptr<MenuMain>> vecChildren;
    MenuSelectableType                     type;     /* Type of an object.                                      */
    std::string                            strLabel; /* Label / Name of the window.                             */

protected:
    virtual void AddDummy();
    virtual void AddCheckBox(std::string strSelectableLabel, bool *bValue);
    virtual void AddButton(std::string strSelectableLabel, void(&fnPointer)(), Vector2D vecButtonSize = Vector2D(0, 0));
    virtual void AddCombo(std::string strSelectableLabel, std::vector<std::string> vecBoxOptions, int* iVecIndex);
    virtual void AddSlider(std::string strLabel, float* flValue, float flMinValue, float flMaxValue);
    virtual void AddSlider(std::string strLabel, int* iValue, int iMinValue, int iMaxValue);

protected:
    bool        bIsHovered;         /* Defines if the selectable is hovered with the mouse cursor.   */
    bool        bLMBPressedLast;
    MenuMain*   pParent;
    float       flMaxChildWidth;    /* Maximum child width. Set mainly for buttons and selectables. */
    Vector2D    vecPosition;        /* Coordinates to top-left corner of the drawed ent.            */
    Vector2D    vecSize;            /* Size of the drawed ent.                                      */
};



class BaseWindow : public MenuMain
{
public:
    BaseWindow() : pHeaderFont(nullptr), iHeaderHeight(0) {};
    BaseWindow(Vector2D vecPosition, Vector2D vecSize, CD3DFont* font, CD3DFont* headerFont, std::string strLabel = "");
    virtual void Render();
    virtual bool UpdateData();

    static bool bIsDragged;         /* Says if the window is currently dragged. Defined as static as its the same for all children */
private:
    virtual int  GetHeaderHeight();

    CD3DFont* pHeaderFont;          /* Header only font         */
    int       iHeaderHeight;        /* Header height in pixels  */
};



class BaseSection : public BaseWindow
{
public:
    BaseSection(Vector2D vecSize, int iNumRows, std::string strLabel);
    virtual void Render();
    virtual bool UpdateData();

private:
    void SetupPositions();          /* Calculates proper positions for all child selectables.   */
    bool bIsInitialized = false;    /* Local boolean for initialization purposes.               */
    int iNumRows;                   /* Number of vertical rows we will split section to.        */

};



class Checkbox : public MenuMain
{
public:
    Checkbox(std::string strLabel, bool *bValue, MenuMain* pParent);
    virtual void Render();
    virtual bool UpdateData();

    bool*   bCheckboxValue;         /* The value we are changing with the checkbox                  */
private:
    Vector2D vecSelectableSize;
    Vector2D vecSelectablePosition;
};



class Button : public MenuMain
{
public:
    Button(std::string strLabel, void(&fnPointer)(), MenuMain* pParent, Vector2D vecButtonSize = Vector2D(0, 0));
    virtual void Render();
    virtual bool UpdateData();
private:
    bool    bIsActivated;           /* Defines if we activated the button                            */
    void    (*fnActionPlay)();      /* Pointer to the function that will be run at the button press. */
};



class ComboBox : public MenuMain
{
public:
    ComboBox(std::string strLabel, std::vector<std::string> vecBoxOptions, int* iCurrentValue, MenuMain* pParent);
    virtual void Render();
    virtual bool UpdateData();

    virtual Vector2D GetSelectableSize();
private:
    bool  bIsActive;                            /* Boolean defining if we are supposed to draw our list or not. */
    bool  bIsButtonHeld;
    int   idHovered;            
    int*  iCurrentValue;                        /* Current selected option. Defined as a vector index.          */
    Vector2D vecSelectableSize;                 /* Size of the internal selectable size of the combo            */
    Vector2D vecSelectablePosition;             /* Position of the selectable                                   */
    std::vector<std::string> vecSelectables;    /* Vector of strings that will appear as diff settings.         */
};


template <typename T>
class Slider : public MenuMain
{
public:
    Slider(const std::string& strLabel, T* flValue, T flMinValue, T flMaxValue, MenuMain* pParent);
    virtual void Render();
    virtual bool UpdateData();

    float GetValuePerPixel() const;
    void  SetValue(T flValue);
private:
    T*    nValue;                               /* Slider current value                              */
    T     nMin;                                 /* Minimal slider value                              */
    T     nMax;                                 /* Maximal slider value                              */
    float flDragX;                              /* Mouse position at the start of the drag           */
    float flDragOffset;                         /* Offset of the mouse position                      */
    float flButtonPosX;
    bool  bPressed;

    Vector2D vecSelectableSize;                 /* Size of the internal selectable size of the combo */
    Vector2D vecSelectablePosition;             /* Position of the selectable                        */
};


class DummySpace : public MenuMain
{
public:
         DummySpace(Vector2D size) { this->vecSize = size; };
    void Render()     override { };
    bool UpdateData() override { return false; };
};

enum class MenuSelectableType
{
    TYPE_MAIN,
    TYPE_WINDOW,
    TYPE_SECTION,
    TYPE_CHECKBOX,
    TYPE_BUTTON,
    TYPE_COMBO,
    TYPE_SLIDER,
};
