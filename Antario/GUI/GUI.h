#pragma once
#include <vector>
#include "..\Utils\DrawManager.h"


namespace ui
{
    /* Predefinition */
    class MenuMain;
    class UIObject;
    class Control;
    class ControlManager;
    class Section;
    class Tab;

    using SSize     = SPoint;
    using TabPtr    = std::shared_ptr<Tab>;
    using ObjectPtr = std::shared_ptr<UIObject>;

    /* MST - Menu Selectable Type, specifies the type of the selectable for easier usage */
    enum MST
    {
        TYPE_INCORR = -1,
        TYPE_WINDOW,
        TYPE_TAB,
        TYPE_SECTION,
        TYPE_CHECKBOX,
        TYPE_BUTTON,
        TYPE_COMBO,
        TYPE_SLIDER,
    };

    struct MenuStyle
    {
        int   iPaddingX       = 20;                     /*- Padding between sections                            -*/
        int   iPaddingY       = 6;                      /*- Padding between selectables                         -*/
        Color colCursor       = { 0, 150, 255, 100 };   /*- Color of the mouse cursor                           -*/
        Color colHover        = { 100, 100, 100, 30 };  /*- Color applied on the obj when mouse hovers above it -*/
        Color colSectionOutl  = { 15, 15, 15, 200 };    /*- Color of the section outline                        -*/
        Color colSectionFill  = { 0, 0, 0, 30 };        /*- Color filling the section bounds                    -*/
        Color colCheckboxFill = { 50, 50, 50, 255 };    /*- Color of the first gradient color of the checkbox   -*/
        Color colMenuStyle    = { 35, 35, 35, 255 };    /*- Color of the menu style controls                    -*/
        Color colText         = { 160, 160, 160, 255 }; /*- Color of the text inside the main window            -*/
        Color colHeader       = { 50, 50, 50, 230 };    /*- Color of the header background                      -*/
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

        SPoint     GetPos() const { return vecPointPos; }; /* Set&get actual mouse position */
        virtual void SetPosition(SPoint ptPos) { this->vecPointPos = ptPos; };

        /* Checks if the mouse is in specified bounds */
        virtual bool IsInBounds(const SPoint& vecDst1, const SPoint& vecDst2);
        virtual bool IsInBounds(const SRect& rcRect);

        bool     bLMBPressed = false; /* Actual state of Left Mouse Button (pressed or not)   */
        bool     bRMBPressed = false; /* Actual state of Right Mouse Button (pressed or not)  */
        bool     bLMBHeld = false;
        bool     bRMBHeld = false;
        SPoint   vecPointPos;         /* Current mouse cursor position                        */
    };


    /* Main menu object - contains all of the data within it such as mouse cursor, windows etc */
    class MenuMain
    {
    public:
        MenuMain() = default;

        virtual void Initialize();
        virtual bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual void Render();                           /* Renders the object                                      */
    
        /* Non-inline, it breaks sharedpointers */
        static MenuStyle style;                          /* Struct containing all colors / paddings in our menu.    */
        static Control*  pFocusedObject;                 /* Pointer to the focused object                           */
        static std::shared_ptr<CD3DFont>    pFont;       /* Pointer to the font used in the menu.                   */
        static std::unique_ptr<MouseCursor> mouseCursor; /* Pointer to our mouse cursor                             */

        std::vector<ObjectPtr> vecChildren;
    };


    /* Virtual main object class */
    class UIObject : public MenuMain, public std::enable_shared_from_this<UIObject>
    {
    public:
        UIObject() : type(MST(-1)), pParent(nullptr), bIsHovered(false), iMaxChildWidth(0) { }

        /* Used to get shared_ptr from thisptr - makes using smart pointers easier */
        ObjectPtr GetThis()                         { return shared_from_this(); }

        /* Handle input */
        bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override              { return false; }
        virtual bool HandleKeyboardInput(UINT uMsg, WPARAM wParam, LPARAM lParam)   { return false; }
        virtual bool HandleMouseInput   (UINT uMsg, WPARAM wParam, LPARAM lParam)   { return false; }

        virtual SPoint GetPos()  const              { return this->rcBoundingBox.Pos(); }
        virtual void   SetPos(SPoint ptNewPosition) { this->rcBoundingBox.left = ptNewPosition.x; rcBoundingBox.top = ptNewPosition.y; }
        virtual SSize  GetSize() const              { return this->szSizeObject; }
        virtual void   SetSize(SSize sNewSize)      { this->szSizeObject = sNewSize; }
        virtual SRect  GetBBox()                    { return this->rcBoundingBox; }

        virtual void SetHovered(bool hov)		{ this->bIsHovered = hov; }
        virtual int GetScrollableHeight() const { return 0; }

        /* Position setups */
        void Initialize() override    { }
        virtual void SetupBaseSize();                            /* Sets the object rect bounds using object size       */
        virtual void SetupPositions() { this->SetupBaseSize(); } /* Internal position setup                             */
        
        /* Parent/child setting functions */

        virtual void      AddChild(ObjectPtr child)     { this->vecChildren.push_back(child); }
        virtual int       GetMaxChildWidth()            { return this->iMaxChildWidth; }
        virtual void      SetParent(ObjectPtr parent)   { this->pParent = parent; };
        virtual ObjectPtr GetParent()                   { return this->pParent; }

        MST type;                 /* Type of an object.                                           */
        std::string strLabel;     /* Label / Name of the window.                                  */

    protected:
        ObjectPtr pParent;        /* Parent object */
        bool      bIsHovered;     /* Defines if the object is hovered with the mouse cursor.      */
        int       iMaxChildWidth; /* Maximum child width. Set mainly for buttons and selectables. */
        SSize     szSizeObject;   /* Size of the drawed object                                    */
        SRect     rcBoundingBox;  /* Bounding box of the drawed ent.                              */
    };


    /* Virtual control class */
    class Control : public UIObject
    {
    public:
        Control() : bIsVisible(false), bIsActive(false), bIsUsable(false) { }

        /* Focus handling */
        virtual bool CanHaveFocus() const { return false; } /* Defines if the object can have focus                 */
        virtual void OnFocusIn()  { }                       /* Action played when the focus aquired                 */
        virtual void OnFocusOut() { }                       /* Action played when the focus is lost                 */
        virtual void RequestFocus();                        /* Makes object request focus - prioritize its input and drawing */
        

        virtual void SetActive(bool bActive)    { this->bIsActive = bActive;    }
        virtual bool GetActive() const          { return this->bIsActive;       }

        virtual void SetUsable(bool bUsable)    { this->bIsUsable = bUsable;    }
        virtual bool GetUsable() const          { return this->bIsUsable;       }

        virtual void SetVisible(bool bVisible)  { this->bIsVisible = bVisible;  }
        virtual bool GetVisible() const         { return this->bIsVisible;      }

    protected:
        bool bIsVisible;    /* Defines if the control is rendered atm           */
        bool bIsActive;     /* Defines if the control is currently in use       */
        bool bIsUsable;     /* Defines if the control can be used at the moment */
    };


    /* Small virtual helper class */
    class ControlManager : public UIObject
    {
    public:
        ControlManager() = default;

        void SetupPositions() override;
        virtual void SetupChildPositions();                 /* SetupPositions but for child objects.                                    */
        virtual void RenderChildObjects();                  /* Renders all of the child objects, without the focused one.               */
        virtual ObjectPtr GetObjectAtPoint(SPoint ptPoint); /* Gets the object that is on the specified point. Used for "hover check".  */
    };


    class Window : public ControlManager
    {
    public:
        Window() : bIsDragged(false), iHeaderHeight(0), tSelectedTab(nullptr), pHeaderFont(nullptr) { }
        Window(const std::string& strLabel, SPoint ptSize, std::shared_ptr<CD3DFont> pMainFont, std::shared_ptr<CD3DFont> pFontHeader);
    
        void Render()           override;
        void Initialize()       override;
        void SetupPositions()   override;
        bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

        int  GetHeaderHeight() const { return this->iHeaderHeight; }

    private:
        bool   bIsDragged;     /* Says if the window is currently dragged. */
        int    iHeaderHeight;  /* Header height in pixels                  */
        TabPtr tSelectedTab;   /* Actual selected window tab               */
        SRect  rcHeader;       /* Header bounding box                      */
        SPoint ptOldMousePos;  /* Old mouse position used for dragging     */
        std::shared_ptr<CD3DFont> pHeaderFont; /* Header only font         */
    };


    class Tab : public ControlManager
    {
    public:
        Tab(const std::string& strTabName, int iNumColumns, ObjectPtr parentWindow);
        void Initialize() override;
        void Render()     override;
        bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

        void SetupChildPositions()  override;
        void SetActive(bool bNewActive) { this->bIsActive  = bNewActive; }

        std::shared_ptr<Section> AddSection(const std::string& strLabel, float flPercSize);
    private:
        int  iColumnCount;
        bool bIsActive;
    };

	class ScrollBar : public Control
	{
	public:
		ScrollBar(ObjectPtr pParentObject);
		void Initialize() override;
		void Render()	  override;
		bool HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		void SetupPositions()	  override;
		bool CanHaveFocus() const override { return true; }

		void UpdateThumbRect();
		int  GetScrollAmmount() const { return this->iScrollAmmount; };
	private:
		void HandleArrowHeldMode();
		int  iScrollAmmount; /* The offset of the initial position            */
		int  iPageSize;      /* How much pixels are rendered in page (height) */
		bool bIsThumbUsed;   /* Defines if the tumb is grabbed                */

		enum ButtonState
		{
			CLEAR,
			CLICKED_UP,
			CLICKED_DOWN,
			HELD_UP,
			HELD_DOWN
		};
		enum HoveredButton
		{
			NONE,
			UP,
			DOWN,
			THUMB,
			SHAFT
		};
		SPoint ptOldMousePos;
		SSize  sizeThumb;

		SRect rcUpButton;
		SRect rcDownButton;
		SRect rcDragThumb;

		ButtonState eState;
		HoveredButton eHoveredButton;
	};

    class Section : public ControlManager
    {
    public:
        Section(const std::string& strLabel, float flPercSize, ObjectPtr parentTab);
        void Render()               override;
        void RenderChildObjects()   override;
        void Initialize()           override;
        bool MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

        void SetupChildPositions()      override;
        int GetScrollableHeight() const override { return iTotalPixelHeight - this->rcBoundingBox.Height(); }

        virtual void AddDummy();
        virtual void AddCheckBox(const std::string& strSelectableLabel, bool * bValue);
        virtual void AddButton(const std::string& strSelectableLabel, void(&fnPointer)(), SPoint ptButtonSize = SPoint(0, 0));
        virtual void AddCombo(const std::string& strSelectableLabel, int* iVecIndex, std::vector<std::string> vecBoxOptions);
        virtual void AddSlider(const std::string& strLabel, float* flValue, float flMinValue, float flMaxValue);
        virtual void AddSlider(const std::string& strLabel, int* iValue, int iMinValue, int iMaxValue);
    protected:
		int   iTotalPixelHeight;
        float flSizeScale;  /* Scale of the window space taken by section */
        SSize sizeSect;     /* Size of the section      */
		std::unique_ptr<ScrollBar> scrollBar;   /* Scrollbar of the section */
    };


    
    class Checkbox : public Control
    {
    public:
        Checkbox(const std::string& strLabel, bool *bValue, ObjectPtr pParent);
        void Render() override;
        void SetupPositions() override;
        bool HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    
        bool*  bCheckboxValue;  /* The value we are changing with the checkbox                  */
    private:
        SRect rcBox;
    };



    class Button : public Control
    {
    public:
        Button(const std::string& strLabel, void(&fnPointer)(), ObjectPtr pParent, SPoint ptButtonSize = SPoint(0, 0));
        void Render() override;
        void Initialize() override;
        bool HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    private:
        bool    bIsActivated;       /* Defines if we activated the button                            */
        void(*fnActionPlay)();      /* Pointer to the function that will be run at the button press. */
    };
    
    
    
    class ComboBox : public Control
    {
    public:
        ComboBox(const std::string& strLabel, std::vector<std::string> vecBoxOptions, int* iCurrentValue, ObjectPtr pParent);
        void Initialize()         override;
        void Render()             override;
        bool HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
        void SetupPositions()     override;
        bool CanHaveFocus() const override { return true; }
        void OnFocusOut()         override { this->bIsActive = false; };
    
        virtual SPoint GetSelectableSize();
        void RenderSelectables();

        int*  iCurrentValue;                        /* Current selected option. Defined as a pttor index.          */
    private:
        int   idHovered;
        SRect rcSelectable;                         /* Selectable bounding box */
        std::vector<std::string> vecSelectables;    /* Vector of strings that will appear as diff settings.         */
    };
    
    
    template <typename T>
    class Slider : public Control
    {
    public:
        Slider(const std::string& strLabel, T* tValue, T tMinValue, T tMaxValue, ObjectPtr pParent);

        void Initialize()           override;
        void Render()               override;
        void SetupPositions()       override;
        bool HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam)    override;
        bool HandleKeyboardInput(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

        bool CanHaveFocus() const   override { return true; }   /* Override so we specify that slider can have focus */

    private:
        float GetValuePerPixel() const; /* Get value change if we move slider 1 pixel wide */
        void  SetValue(T flValue);      /* Set slider value to desired one                 */
        int   GetZeroPos();             /* Get the "beginning" of our slider fill value    */
            
        T*   nValue;        /* Slider current value                    */
        T    nMin;          /* Minimal slider value                    */
        T    nMax;          /* Maximal slider value                    */
        int  iDragX{};      /* Mouse position at the start of the drag */
        int  iDragOffset{}; /* Offset of the mouse position            */
        int  iButtonPosX{}; /* Slider button representing value        */
        bool bPressed{};             
        SRect rcSelectable; /* Size of the internal selectable size of the combo */
    };
    
    
    class DummySpace : public Control
    {
    public:
        DummySpace(SSize size) { this->szSizeObject = size; }
        void Render() override { }
    };
}
