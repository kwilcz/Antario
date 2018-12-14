#include "GUI.h"
#include "..\Settings.h"

#include <execution>
#include <algorithm>


using namespace ui;

// Defined to avoid including windowsx.h
#define GET_X_LPARAM(lp)                        (int(short(LOWORD(lp))))
#define GET_Y_LPARAM(lp)                        (int(short(HIWORD(lp))))

/* No inline vars for shared pointers I guess */
MenuStyle MenuMain::style;                          /* Struct containing all colors / paddings in our menu.    */
Control*  MenuMain::pFocusedObject;                 /* Pointer to the focused object                           */
std::shared_ptr<CD3DFont>    MenuMain::pFont;       /* Pointer to the font used in the menu.                   */
std::unique_ptr<MouseCursor> MenuMain::mouseCursor; /* Pointer to our mouse cursor                             */


void MouseCursor::Render()
{
    const auto x = this->vecPointPos.x;
    const auto y = this->vecPointPos.y;

    ///TODO: render this fucker to texture first and stop wasting time rendering that
    // Draw inner fill color
    SPoint ptPos1 = { x + 1,  y + 1 };
    SPoint ptPos2 = { x + 25, y + 12 };
    SPoint ptPos3 = { x + 12, y + 25 };
    g_Render.TriangleFilled(ptPos1, ptPos2, ptPos3, MenuMain::style.colCursor);

    // Draw second smaller inner fill color
    ptPos1 = { x + 6,  y + 6 };
    ptPos2 = { x + 19, y + 12 };
    ptPos3 = { x + 12, y + 19 };
    g_Render.TriangleFilled(ptPos1, ptPos2, ptPos3, MenuMain::style.colCursor);

    // Draw border
    g_Render.Triangle({ x, y }, { x + 25, y + 12 }, { x + 12, y + 25 }, Color::Black(200));
}


void MouseCursor::RunThink(const UINT uMsg, const LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
    case WM_NCMOUSEMOVE:
        this->SetPosition(SPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
        break;
    case WM_LBUTTONDOWN:
        this->bLMBPressed = true;
        break;
    case WM_LBUTTONUP:
        this->bLMBHeld = false;
        this->bLMBPressed = false;
        break;
    case WM_RBUTTONDOWN:
        this->bRMBPressed = true;
        break;
    case WM_RBUTTONUP:
        this->bRMBHeld = false;
        this->bRMBPressed = false;
        break;
    default:
        break;
    }
    if (this->bLMBPressed)
    {
        if (this->bLMBHeld)
            this->bLMBPressed = false;

        this->bLMBHeld = true;
    }
    if (this->bRMBPressed)
    {
        if (this->bRMBHeld)
            this->bRMBPressed = false;
        this->bRMBHeld = true;
    }
}

bool MouseCursor::IsInBounds(const SPoint& vecDst1, const SPoint& vecDst2)
{
    return this->IsInBounds({ vecDst1, vecDst2 });
}


bool MouseCursor::IsInBounds(const SRect& rcRect)
{
    return rcRect.ContainsPoint(this->GetPos());
}


void UIObject::SetupBaseSize()
{
    this->rcBoundingBox.right  = rcBoundingBox.left + szSizeObject.x;
    this->rcBoundingBox.bottom = rcBoundingBox.top  + szSizeObject.y;
}

void Control::RequestFocus()
{
    if (pFocusedObject == this)
        return;

    if (!this->CanHaveFocus())
        return;

    if (pFocusedObject)
        pFocusedObject->OnFocusOut();

    pFocusedObject = dynamic_cast<Control*>(this);
    pFocusedObject->OnFocusIn();
}

void MenuMain::Render()
{
    /*  Render all children */
    for (auto& it : this->vecChildren)
        if (it.get() != pFocusedObject)
            it->Render();

    /* Render focused object as the last one */
    if (pFocusedObject)
        pFocusedObject->Render();
}

bool MenuMain::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    mouseCursor->RunThink(uMsg, lParam);

    /* Loop through all of the child objects and capture the input */
    if (!this->vecChildren.empty() && g_Settings.bMenuOpened)
    {
        for (auto& it : this->vecChildren)
            if (it->MsgProc(uMsg, wParam, lParam))
                return true;
    }
    return false;
}

void Section::AddDummy()
{
    this->AddChild(std::make_shared<DummySpace>(SPoint(this->GetMaxChildWidth(), pFont->iHeight + style.iPaddingY)));
}

void Section::AddCheckBox(const std::string& strSelectableLabel, bool* bValue)
{
    this->AddChild(std::make_shared<Checkbox>(strSelectableLabel, bValue, GetThis()));
}

void Section::AddButton(const std::string& strSelectableLabel, void(&fnPointer)(), SPoint vecButtonSize)
{
    this->AddChild(std::make_shared<Button>(strSelectableLabel, fnPointer, GetThis(), vecButtonSize));
}

void Section::AddCombo(const std::string& strSelectableLabel, int* iVecIndex, std::vector<std::string> vecBoxOptions)
{
    this->AddChild(std::make_shared<ComboBox>(strSelectableLabel, vecBoxOptions, iVecIndex, GetThis()));
}

void Section::AddSlider(const std::string& strLabel, float* flValue, float flMinValue, float flMaxValue)
{
    this->AddChild(std::make_shared<Slider<float>>(strLabel, flValue, flMinValue, flMaxValue, GetThis()));
}

void Section::AddSlider(const std::string& strLabel, int* iValue, int iMinValue, int iMaxValue)
{
    this->AddChild(std::make_shared<Slider<int>>(strLabel, iValue, iMinValue, iMaxValue, GetThis()));
}


ObjectPtr ControlManager::GetObjectAtPoint(SPoint ptPoint)
{
    ObjectPtr returnObject = nullptr;
    std::for_each(std::execution::par, vecChildren.begin(), vecChildren.end(), 
                [&ptPoint, &returnObject](ObjectPtr& object)
    {
        if (object->GetBBox().ContainsPoint(ptPoint))
        {
            object->SetHovered(true);
            returnObject = object;
        }
        else object->SetHovered(false);
    });
    return returnObject;
}


void ControlManager::RenderChildObjects()
{
    for (auto& it : this->vecChildren)
        if (it.get() != pFocusedObject)
            it->Render();
}


void ControlManager::SetupPositions()
{
    this->SetupBaseSize();
    this->SetupChildPositions();
}


void ControlManager::SetupChildPositions()
{
    std::for_each(std::execution::par, vecChildren.begin(), vecChildren.end(),
        [](ObjectPtr& object)
    {
        object->SetupPositions();
    });
}


Window::Window(const std::string& strLabel, SPoint szSize, std::shared_ptr<CD3DFont> pMainFont, std::shared_ptr<CD3DFont> pFontHeader)
{
    this->pFont          = pMainFont;
    this->pHeaderFont    = pFontHeader;
    this->strLabel       = strLabel;
    this->szSizeObject   = szSize;
    this->bIsDragged     = false;

    this->iHeaderHeight = pFont->iHeight + 2;
    UIObject::SetPos(g_Render.GetScreenCenter() - (szSize * .5f));
    this->type = TYPE_WINDOW;
}

void Window::Render()
{
    // Draw main background rectangle
    g_Render.RectFilledGradient(this->rcBoundingBox, Color(50, 50, 50, 255), Color(20, 20, 20, 235), GradientType::GRADIENT_VERTICAL);

    // Draw header rect.
    g_Render.RectFilledGradient(this->rcBoundingBox.Pos(), { this->rcBoundingBox.right, this->rcBoundingBox.top + this->iHeaderHeight }, style.colHeader,
        Color(35, 35, 35, 230), GradientType::GRADIENT_VERTICAL);

    // Draw header string, defined as label.
    g_Render.String(this->rcBoundingBox.Mid().x, this->rcBoundingBox.top + int(float(iHeaderHeight) * 0.5f), CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y,
                    style.colHeaderText, this->pHeaderFont.get(), this->strLabel.c_str());

    // Render all childrens
    this->RenderChildObjects();
}


void Window::Initialize()
{
    this->tSelectedTab = std::dynamic_pointer_cast<Tab>(*vecChildren.begin());  /* Setup the first selected tab as the first obj. */
    this->tSelectedTab->SetActive(true);
    this->SetupPositions();
    for (auto& it : vecChildren)
        it->Initialize();
}


void Window::SetupPositions()
{
    /* Set base window rect */
    this->SetupBaseSize();
    this->rcHeader = 
    {
        this->rcBoundingBox.left,
        this->rcBoundingBox.top,
        this->rcBoundingBox.right,
        this->rcBoundingBox.top + this->iHeaderHeight
    };

    /* Make tabs take up all of aviable window width */
    const int iSingleTabWidth = rcBoundingBox.Width() / vecChildren.size();

    /* Setup tab positions */
    int iUsedSpace = 0;
    for (auto& it : vecChildren)
    {
        it->SetSize({ iSingleTabWidth, pFont->iHeight + 2 });
        it->SetPos({ this->GetPos().x + iUsedSpace, this->GetPos().y + iHeaderHeight });
        iUsedSpace += iSingleTabWidth;
        it->SetupPositions();
    }
}


bool Window::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (this->ptOldMousePos == SPoint(0, 0))
        this->ptOldMousePos = mouseCursor->GetPos();

    switch (uMsg)
    {
        case WM_MOUSEMOVE:
            /* Handle window dragging */
            if (this->bIsDragged)
            {
                this->SetPos(this->GetPos() + mouseCursor->GetPos() - this->ptOldMousePos);
                this->SetupPositions();
                return true;
            }
        case WM_LBUTTONDOWN:
        {
            /* Check what tab mouse is hovering */
            auto tHoveredTab = std::dynamic_pointer_cast<Tab>(this->GetObjectAtPoint(mouseCursor->GetPos()));
            if (tHoveredTab)
            {
                /* Handle tab selection */
                if (mouseCursor->bLMBPressed)
                {
                    tSelectedTab->SetActive(false);
                    tHoveredTab->SetActive(true);
                    tSelectedTab = tHoveredTab;
                    /* Clear focus on tabchange */
                    if (pFocusedObject)
                    {
                        pFocusedObject->OnFocusOut();
                        pFocusedObject = nullptr;
                    }
                    return true;
                }
            }
            /* Check if the window is dragged. If it is, move window by the cursor difference between ticks. */
            if (this->rcHeader.ContainsPoint(mouseCursor->GetPos()) && uMsg == WM_LBUTTONDOWN)
            {
                this->bIsDragged = true;
                return true;
            }
            break;
        }
        case WM_LBUTTONUP:
            this->bIsDragged = false;
            break;
    }

    this->ptOldMousePos = mouseCursor->GetPos();

    /* Run MsgProc for selected tab if we did't capture input yet */
    return this->tSelectedTab->MsgProc(uMsg, wParam, lParam);
}

std::shared_ptr<Section> Tab::AddSection(const std::string& strLabel, float flPercSize)
{
    auto tmp = std::make_shared<Section>(strLabel, flPercSize, GetThis());
    this->AddChild(tmp);
    tmp->SetParent(GetThis());
    return tmp;
}

ScrollBar::ScrollBar(ObjectPtr pParentObject)
{
	this->pParent = pParentObject;
	this->szSizeObject.x = 8;
	this->iPageSize = 0;
	this->iScrollAmmount = 0;
	this->eState = CLEAR;
	this->bIsVisible = true;/* For initials checks */
}


void ScrollBar::Initialize()
{
	this->szSizeObject.y = pParent->GetSize().y - 2;
	this->SetupPositions();
}


void ScrollBar::Render()
{
	if (!this->bIsVisible)
		return;

	/* Up/down button */
	g_Render.RectFilled(this->rcUpButton, style.colCheckboxFill);
	g_Render.RectFilled(this->rcDownButton, style.colCheckboxFill);

	/* Drag thumb */
	g_Render.RectFilled(this->rcDragThumb, style.colCheckboxFill);

	/* Has to be here as wndproc is not called when you hold lmb and not do anything else */
	this->HandleArrowHeldMode();
}


bool ScrollBar::HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!this->bIsVisible)
		return false;
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		if (mouseCursor->IsInBounds(rcDragThumb))
			eHoveredButton = THUMB;
		else if (mouseCursor->IsInBounds(rcUpButton))
			eHoveredButton = UP;
		else if (mouseCursor->IsInBounds(rcDownButton))
			eHoveredButton = DOWN;
		else if (mouseCursor->IsInBounds(rcBoundingBox))
			eHoveredButton = SHAFT;
		else
			eHoveredButton = NONE;

		bIsHovered = eHoveredButton != NONE;
	}
	case WM_LBUTTONDOWN:
	{
		if (!mouseCursor->bLMBHeld)
			bIsThumbUsed = false;

		if (bIsHovered && uMsg == WM_LBUTTONDOWN)
		{
			/* Handle button behaviour */
			switch (eHoveredButton)
			{
			case THUMB:
			{
				if (mouseCursor->bLMBPressed)
				{
					this->RequestFocus();
					bIsThumbUsed = true;
				}
				break;
			}
			case UP:
			{
				if (mouseCursor->bLMBPressed)
				{
					this->RequestFocus();

					this->eState = CLICKED_UP;
					this->iScrollAmmount -= 1;
					UpdateThumbRect();
					this->pParent->SetupPositions();
					return true;
				}
				break;
			}
			case DOWN:
			{
				if (mouseCursor->bLMBPressed)
				{
					this->RequestFocus();

					this->eState = CLICKED_DOWN;
					this->iScrollAmmount += 1;
					UpdateThumbRect();
					this->pParent->SetupPositions();
					return true;
				}
				break;
			}
			case SHAFT:
			{
				if (mouseCursor->bLMBPressed)
				{
					this->RequestFocus();
					this->eState = CLEAR;
					this->iScrollAmmount += mouseCursor->GetPos().y > rcDragThumb.top ? rcDragThumb.Height() : -rcDragThumb.Height();
					UpdateThumbRect();
					this->pParent->SetupPositions();
					return true;
				}
			}
			case NONE:
				this->eState = CLEAR;
				break;
			}
		}
		if (bIsThumbUsed)
		{
			if (ptOldMousePos == SPoint(0, 0))
				ptOldMousePos = mouseCursor->GetPos();

			/* Change the scroll ammount by difference in pixels of the old and new mousepos */
			iScrollAmmount += mouseCursor->GetPos().y - ptOldMousePos.y;
			UpdateThumbRect();
			this->pParent->SetupPositions();

			return true;
		}
	}
	break;
	case WM_MOUSEWHEEL:
	{
		if (mouseCursor->IsInBounds(pParent->GetBBox()))
		{
			this->RequestFocus();
			this->iScrollAmmount -= 10 * int(float(GET_WHEEL_DELTA_WPARAM(wParam)) / float(WHEEL_DELTA));
			UpdateThumbRect();
			this->pParent->SetupPositions();
			return true;
		}
	}
	}

	ptOldMousePos = mouseCursor->GetPos();
	return false;
}


void ScrollBar::SetupPositions()
{
	if (!this->bIsVisible)
		return;

	this->iPageSize = pParent->GetSize().y - style.iPaddingY * 2;
	this->rcBoundingBox.left = pParent->GetBBox().right - szSizeObject.x - 2;
	this->rcBoundingBox.right = rcBoundingBox.left + szSizeObject.x;
	this->rcBoundingBox.top = pParent->GetBBox().top + 1;
	this->rcBoundingBox.bottom = rcBoundingBox.top + szSizeObject.y;

	this->rcUpButton = { rcBoundingBox.left, rcBoundingBox.top, rcBoundingBox.right, rcBoundingBox.top + szSizeObject.x };
	this->rcDownButton = { rcBoundingBox.left, rcBoundingBox.bottom - szSizeObject.x, rcBoundingBox.right, rcBoundingBox.bottom };
	this->rcDragThumb.left = rcUpButton.left;
	this->rcDragThumb.right = rcUpButton.right;

	/* Thumb size, -4 cus of space between top and the bottom button */
	this->sizeThumb = { szSizeObject.x, rcDownButton.top - rcUpButton.bottom - pParent->GetScrollableHeight() - 4 };
	UpdateThumbRect();
}


void ScrollBar::UpdateThumbRect()
{
	const auto iScrollableHeight = pParent->GetScrollableHeight();

	if (iScrollableHeight <= 0)
	{
		/* Nothing to scroll through */
		rcDragThumb.top = rcUpButton.bottom + 2;
		rcDragThumb.bottom = rcDownButton.top - 2;
		iScrollAmmount = 0;
	}
	else
	{
		iScrollAmmount = std::clamp(iScrollAmmount, 0, iScrollableHeight);
		rcDragThumb.top = rcUpButton.bottom + 2 + iScrollAmmount;
		rcDragThumb.bottom = rcDragThumb.top + sizeThumb.y;
	}
}


void ScrollBar::HandleArrowHeldMode()
{
	if (mouseCursor->bLMBHeld)
	{
		switch (eHoveredButton)
		{
		case UP:
		{
			this->eState = HELD_UP;
			this->iScrollAmmount -= 1;
			UpdateThumbRect();
			this->pParent->SetupPositions();
			break;
		}
		case DOWN:
		{
			this->eState = HELD_DOWN;
			this->iScrollAmmount += 1;
			UpdateThumbRect();
			this->pParent->SetupPositions();
			break;
		}
		}
	}
}


Tab::Tab(const std::string& strTabName, int iNumColumns, ObjectPtr parentWindow)
{
    this->bIsHovered   = false;
    this->bIsActive    = false;
    this->iColumnCount = iNumColumns;
    this->strLabel     = strTabName;
    this->pParent      = parentWindow;
    this->type         = TYPE_TAB;
}


void Tab::Initialize()
{
    /* Remove padding from aviable section space */
    const auto iAvWidthForSection = pParent->GetBBox().Width() - style.iPaddingX * (iColumnCount - 1) - 2 * style.iPaddingX;
    this->iMaxChildWidth = iAvWidthForSection / iColumnCount;

    std::for_each(std::execution::par, vecChildren.begin(), vecChildren.end(), 
                [](ObjectPtr it) {it->Initialize(); });

    SetupPositions();
}


void Tab::Render()
{
    /* Tab inside, for now menustyle color */
    g_Render.RectFilled(rcBoundingBox, style.colMenuStyle);

    /* Tab outline */
    g_Render.Rect(rcBoundingBox, style.colSectionOutl);

    /* Render tab name */
    g_Render.String(rcBoundingBox.Mid(), CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y | CD3DFONT_DROPSHADOW, style.colText(bIsActive ? 255 : 150), pFont.get(), strLabel.c_str());

    

    if (this->bIsHovered)
        g_Render.RectFilled(rcBoundingBox, style.colHover);

    /* Render sections */
    if (this->bIsActive)
        this->RenderChildObjects();
}


bool Tab::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (this->bIsActive)
    {
        for (auto& it : this->vecChildren)
            if (it->MsgProc(uMsg, wParam, lParam))
                return true;
    }
    return false;
}


void Tab::SetupChildPositions()
{
    int iUsedArea   = 0,
        iPosX       = pParent->GetBBox().left + style.iPaddingX;
    const int iPosY = this->GetBBox().bottom  + style.iPaddingY;

    for (auto& it : this->vecChildren)
    {
        [this, &iUsedArea, &iPosX, &it](int posy)
        {
            posy += iUsedArea;

            /* If section exceeds the bounds move it to the right */
            if (posy + it->GetSize().y > pParent->GetBBox().bottom)
            {
                posy -= iUsedArea;
                iUsedArea = 0;
                iPosX += GetMaxChildWidth() + style.iPaddingX;
            }

            it->SetPos({ iPosX, posy });
            iUsedArea += it->GetSize().y + style.iPaddingY;

            it.get()->SetupPositions();
        }(iPosY);
    }
}


Section::Section(const std::string& strLabel, float flPercSize, ObjectPtr parentTab)
{
    this->pParent = parentTab;
    this->strLabel = strLabel;
    this->type = TYPE_SECTION;

    flPercSize = std::clamp(flPercSize, 0.f, 1.f);
    this->flSizeScale = flPercSize;
}

void Section::Render()
{
    g_Render.RectFilled(this->rcBoundingBox, style.colSectionFill);
    g_Render.Rect(this->rcBoundingBox, style.colSectionOutl);

    g_Render.SetCustomScissorRect(this->rcBoundingBox);
    {
        scrollBar->Render();
        this->RenderChildObjects();
    }
    g_Render.RestoreOriginalScissorRect();
}


void Section::RenderChildObjects()
{
    for (auto& it : this->vecChildren)
    {
        auto control = std::dynamic_pointer_cast<Control>(it);
        if (control.get() != pFocusedObject && control->GetVisible())
            control->Render();
    }
}


void Section::Initialize()
{
    ///TODO: Calc height using prev. calculated amount of sections in a column, we need to know how many paddings we need to redistribute between them
    this->szSizeObject =
    {
        pParent->GetMaxChildWidth(),
        int(float(pParent->GetParent()->GetBBox().Height() - pParent->GetBBox().Height() - std::dynamic_pointer_cast<Window>(pParent->GetParent())->GetHeaderHeight()) * flSizeScale) - 2 * style.iPaddingY
    };

    scrollBar->Initialize();
    this->iMaxChildWidth = this->szSizeObject.x - 2 * style.iPaddingX - scrollBar->GetBBox().Width();
    for (auto& it : vecChildren)
        it->Initialize();

    SetupPositions();

    /* If there are not enough controls for scrollbar to be useful - disable it */
    if (this->GetScrollableHeight() <= 0)
        this->scrollBar->SetVisible(false);
}


bool Section::MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    /* Let the scrollbar handle input first */
    if (this->scrollBar->HandleMouseInput(uMsg, wParam, lParam))
        return true;


    /* Then let the focused control handle full msgproc if defined (colorpickers / any other popup windows) */
    if (pFocusedObject && GetThis() == pFocusedObject->GetParent())
        if (pFocusedObject->MsgProc(uMsg, wParam, lParam))
            return true;

    /* And after that all the other controls */
    switch (uMsg)
    {

    /* Keyboard input */
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    case WM_CHAR:
    {
        /* Let the focused control handle the input, no focus = no keyboard handling */
        if (pFocusedObject)
            if (pFocusedObject->HandleKeyboardInput(uMsg, wParam, lParam))
                return true;
        break;
    }

    /* Mouse input */
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_XBUTTONDBLCLK:
    case WM_MOUSEWHEEL:
    {
        /* Let the focused object handle mouse input before the rest */
        if (pFocusedObject)
            if (pFocusedObject->HandleMouseInput(uMsg, wParam, lParam))
                return true;

        bool bHandledInput = false;
        auto pControl = this->GetObjectAtPoint(mouseCursor->GetPos());

        /* If hovered control is not the focused one, we didnt capture input for it yet so do it */
        if (pControl.get() != pFocusedObject)
        {
            const auto oldFocus = pFocusedObject;

            if (pControl)
                if (pControl->HandleMouseInput(uMsg, wParam, lParam))
                    bHandledInput = true;

            /* Our control changed focused object */
            if (oldFocus != pFocusedObject)
                return bHandledInput;

            /* If the new object is not a focused one OR you press LMB out-of-bouds of focused obj, LOSE FOCUS */
            if (uMsg == WM_LBUTTONDOWN && pFocusedObject)
            {
                if ((bHandledInput && pFocusedObject != pControl.get()) || (!bHandledInput && GetThis() == pFocusedObject->GetParent()))
                {
                    pFocusedObject->OnFocusOut();
                    pFocusedObject = nullptr;
                }
            }
        }
        return bHandledInput;
    }
    }

    return false;
}


void Section::SetupChildPositions()
{
    /* Create scrollbar object, this function is called first at init - thats why here */
    if (!scrollBar)
        this->scrollBar = std::make_unique<ScrollBar>(GetThis());

    /* Setup positions of out scrollbar */
    this->scrollBar->SetupPositions(); 

    /* Saved used area of section. Used for control alignment. */
    int iUsedArea = 0 - this->scrollBar->GetScrollAmmount();
    for (auto& it : vecChildren)
    {
        /* useful cast for later use */
        auto control = std::dynamic_pointer_cast<Control>(it);

        const int iPosX = this->rcBoundingBox.left + style.iPaddingX,
                  iPosY = this->rcBoundingBox.top  + style.iPaddingY + iUsedArea;


        /* Check if we will exceed bounds of the section and do not render the selectable */
        if (iPosY > this->GetBBox().bottom || iPosY + control->GetSize().y < this->GetPos().y)
        {
            control->SetVisible(false);
            control->SetUsable(false);
        }
        else
        {
            /* If any part of the control is out-of-bounds, render it but dont capture input */
            control->SetVisible(true);

            if (iPosY + it->GetSize().y > this->GetBBox().bottom || iPosY < this->GetPos().y)
                control->SetUsable(false);
            else
                control->SetUsable(true);
        }

        it->SetPos({ iPosX, iPosY });
        it->SetupPositions();

        iUsedArea += it->GetSize().y + style.iPaddingY;
    }
    /* Save full section height - all controls etc. */
    this->iTotalPixelHeight = iUsedArea + this->scrollBar->GetScrollAmmount() + style.iPaddingY;
}


Checkbox::Checkbox(const std::string& strLabel, bool *bValue, ObjectPtr pParent)
{
    this->pParent        = pParent;
    this->strLabel       = strLabel;
    this->bCheckboxValue = bValue;
    this->szSizeObject   = { 100, int(float(pFont->iHeight) * 1.5) };
    this->type           = TYPE_CHECKBOX;
}

void Checkbox::Render()
{
    /* Checkbox background */
    g_Render.RectFilled(this->rcBox, style.colCheckboxFill);

    /* Fill the inside of the button depending on activation */
    if (*this->bCheckboxValue)
        g_Render.RectFilled(this->rcBox, style.colMenuStyle);

    /* If the button is hovered, make it lighter */
    if (this->bIsHovered)
        g_Render.RectFilled(rcBox, style.colHover);

    /* Render the outline */
    g_Render.Rect(this->rcBox, { 15, 15, 15, 220 });

    /* Render button label as its name */
    g_Render.String(this->rcBox.right + int(float(style.iPaddingX) * 0.5f), this->rcBoundingBox.Mid().y,
                    CD3DFONT_DROPSHADOW | CD3DFONT_CENTERED_Y, style.colText, pFont.get(), this->strLabel.c_str());

}


bool Checkbox::HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        if (mouseCursor->bLMBPressed)
        {
            /* Flip the checkbox value on LMButton press */
            *this->bCheckboxValue = !*this->bCheckboxValue;
            return true;
        }
    }
    return false;
}


void Checkbox::SetupPositions()
{
    this->SetupBaseSize();

    this->rcBox = [this]()
    {
        auto size = int(float(rcBoundingBox.Height()) * 0.50f);
        auto diff = (this->rcBoundingBox.Height() - size) / 2;
        return SRect(rcBoundingBox.left + diff, rcBoundingBox.top + diff, rcBoundingBox.left + diff + size, rcBoundingBox.bottom - diff);
    }();
}



Button::Button(const std::string& strLabel, void(&fnPointer)(), ObjectPtr pParent, SPoint ptButtonSize)
{
    this->pParent      = pParent;
    this->strLabel     = strLabel;
    this->fnActionPlay = fnPointer;
    this->bIsActivated = false;
    this->type         = TYPE_BUTTON;

    /* Will be overriden on the init if == 0, 0 */
    this->szSizeObject = ptButtonSize;
}

void Button::Render()
{
    /* Fill the body of the button */
    g_Render.RectFilled(this->rcBoundingBox, style.colMenuStyle);

    /* Button outline */
    g_Render.Rect(this->rcBoundingBox, style.colSectionOutl);

    /* Text inside the button */
    g_Render.String(this->rcBoundingBox.Mid(), CD3DFONT_DROPSHADOW | CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y, style.colText, pFont.get(), this->strLabel.c_str());
    
    if (this->bIsHovered)
        g_Render.RectFilled(this->rcBoundingBox, style.colHover);
}


void Button::Initialize()
{
    /* Basically override if the size was not specified, its run only once anyway */
    if (this->szSizeObject == SSize(0, 0))
        szSizeObject = { this->pParent->GetMaxChildWidth(),  pFont->iHeight + style.iPaddingY };
}


bool Button::HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        if (mouseCursor->bLMBPressed)
        {
            this->fnActionPlay(); /* Run the function passed as an arg. */
            return true;
        }
    }
    return false;
}





ComboBox::ComboBox(const std::string& strLabel, std::vector<std::string> vecBoxOptions, int* iCurrentValue, ObjectPtr pParent)
{
    this->type      = TYPE_COMBO;
    this->pParent   = pParent;
    this->strLabel  = strLabel;
    this->idHovered = -1;
    this->iCurrentValue  = iCurrentValue;
    this->vecSelectables = vecBoxOptions;
}


void ComboBox::Initialize()
{
    this->szSizeObject.x = this->pParent->GetMaxChildWidth();
    this->szSizeObject.y = int(pFont->iHeight + float(style.iPaddingY) * 0.5f) * 2;    
}


void ComboBox::Render()
{
    /* Render the label (name) above the combo */
    g_Render.String(this->rcBoundingBox.Pos(), CD3DFONT_DROPSHADOW, style.colText, pFont.get(), this->strLabel.c_str());


    /* Render the selectable with the value in the middle and highlight if hovered */
    if (this->bIsHovered)
        g_Render.RectFilled(this->rcSelectable, style.colHover);
    else
        g_Render.RectFilled(this->rcSelectable, style.colComboBoxRect);

    /* Render the selectable with the value in the middle */
    g_Render.String(this->rcSelectable.Mid(), CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y, style.colText, pFont.get(),
                    this->vecSelectables[*this->iCurrentValue].c_str());

    /* Render the small triangle */
    {
        SPoint ptPosMid, ptPosLeft, ptPosRight;

        /* I know, hardcode. You should change this anyway */
        ptPosMid.x   = this->rcSelectable.right - 8;
        ptPosRight.x = this->rcSelectable.right - 5;
        ptPosLeft.x  = this->rcSelectable.right - 11;

        /* Draw two different versions (top-down, down-top) depending on activation */
        if (!this->bIsActive)
        {
            ptPosRight.y = ptPosLeft.y = this->rcSelectable.top + 6;
            ptPosMid.y   = this->rcSelectable.bottom - 6;
        }
        else
        {
            ptPosRight.y = ptPosLeft.y = this->rcSelectable.bottom - 6;
            ptPosMid.y   = this->rcSelectable.top + 6;
        }

        g_Render.TriangleFilled(ptPosLeft, ptPosRight, ptPosMid, style.colText);
        g_Render.Triangle(ptPosLeft, ptPosRight, ptPosMid, style.colText);
    }/*-------------------------*/

    /* Rectangle of the combo selectables, for scissorrect */
    const SRect vpCombo = 
    {
        rcBoundingBox.left,
        rcBoundingBox.bottom,
        rcBoundingBox.right,
        rcSelectable.bottom + rcSelectable.Height() * int(vecSelectables.size())
    };

    /* Render selectables only within the rect, useful for scroll usage (if implemented later on) */
    g_Render.SetCustomScissorRect(vpCombo);
        if (this->bIsActive)
            RenderSelectables();
    g_Render.RestoreOriginalScissorRect();

}


bool ComboBox::HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        {
            /* If the main selectabled is hovered, handle only its input */
            if (this->bIsHovered)
            {
                this->idHovered = -1;
                if (mouseCursor->bLMBPressed)
                {
                    this->bIsActive = !bIsActive;
                    /* If active, request focus so its rendered on top of every other selectable */
                    if (this->bIsActive)
                        this->RequestFocus();
                    return true;
                }
            }
            else if (this->bIsActive &&
                     mouseCursor->IsInBounds({ this->rcSelectable.left, this->rcSelectable.bottom, this->rcSelectable.right,
                                               this->rcSelectable.bottom + this->rcSelectable.Height() * int(this->vecSelectables.size()) }))
            {
                for (std::size_t it = 0; it < this->vecSelectables.size(); ++it)
                {
                    /* Bounds of the looped element. */
                    const auto rcElementBounds = [this, it]()
                    {
                        int posy = this->rcSelectable.bottom + this->rcSelectable.Height() * it;
                        return SRect(this->rcSelectable.left, posy, this->rcSelectable.right, posy + this->rcSelectable.Height());
                    };

                    /* If we don't hover the element - ignore */
                    if (!mouseCursor->IsInBounds(rcElementBounds()))
                        continue;

                    this->idHovered = it;
                    if (mouseCursor->bLMBPressed)
                    {
                        *this->iCurrentValue = it;
                        this->idHovered      = -1;
                        this->bIsActive      = false;
                        return true;
                    }
                }
            }
            else
                this->idHovered = -1;
        }
    }

    return false;
}


void ComboBox::SetupPositions()
{
    this->SetupBaseSize();

    this->rcSelectable = [this]()
    {
        int posy = this->rcBoundingBox.top + pFont->iHeight + int(float(style.iPaddingY) * 0.5f);
        return SRect(this->rcBoundingBox.left, posy, this->rcBoundingBox.right, posy + pFont->iHeight + int(float(style.iPaddingY) * 0.5f));
    }();
}


SPoint ComboBox::GetSelectableSize()
{
    SPoint vecTmpSize;
    vecTmpSize.y = pFont->iHeight + int(float(style.iPaddingY) * 0.5f);
    vecTmpSize.x = this->GetSize().x;
    return vecTmpSize;
}


void ComboBox::RenderSelectables()
{
     /* Background square for the list */
    g_Render.RectFilled({ this->rcSelectable.left, this->rcSelectable.bottom }, 
                        { this->rcSelectable.right, this->rcSelectable.bottom + this->rcSelectable.Height() * int(this->vecSelectables.size()) }, 
                          style.colCheckboxFill);


    /* Distinction line at top for seperation for dropdown */
    g_Render.Line({ this->rcSelectable.left, this->rcSelectable.bottom },
                  { this->rcSelectable.right, this->rcSelectable.bottom }, style.colSectionFill);

    /* Highlight selectable if its selected (-1 = no hovered one) */
    if (this->idHovered != -1)
    {
        const auto rcElementBounds = [this]() -> SRect
        {
            int posy = this->rcSelectable.bottom + this->rcSelectable.Height() * this->idHovered;
            return {
                this->rcSelectable.left + 1,
                posy + 1,
                this->rcSelectable.right - 1,
                posy + this->rcSelectable.Height() - 1
            };
        };

        g_Render.RectFilled(rcElementBounds(), style.colHover);
    }

    /* Render all of the selectable labels (names) in the middle */
    auto index = 0;
    const auto ptMid = this->rcSelectable.Mid();
    for (const auto& it : vecSelectables)
        g_Render.String({ ptMid.x, ptMid.y + this->rcSelectable.Height() * (index++ + 1) },
                        CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y, style.colText, pFont.get(), it.c_str());
}


template<typename T>
Slider<T>::Slider(const std::string& strLabel, T* tValue, T tMinValue, T tMaxValue, ObjectPtr pParent)
{
    this->pParent  = pParent;
    this->strLabel = strLabel;
    this->nValue   = tValue;
    this->nMin     = tMinValue;
    this->nMax     = tMaxValue;
    this->type     = TYPE_SLIDER;

}


template <typename T>
void Slider<T>::Initialize()
{
    this->szSizeObject.x = this->pParent->GetMaxChildWidth();
    this->szSizeObject.y = (pFont->iHeight + int(float(style.iPaddingY) * 0.5f)) * 1.75f;
    const SSize szSelectableSize = { this->szSizeObject.x, pFont->iHeight + int(float(style.iPaddingY) * 0.5f) };

    this->SetValue(*nValue);   // Since its limited, it should not be any higher - even when set in settings before.
}


template<typename T>
void Slider<T>::Render()
{
    std::stringstream ssToRender;
    ssToRender << strLabel << ": " << std::fixed << std::setprecision(2) << *this->nValue;

    /* Render the label (name) above the combo */
    g_Render.String(this->rcBoundingBox.Pos(), CD3DFONT_DROPSHADOW, style.colText, pFont.get(), ssToRender.str().c_str());

    /* Render the selectable with the value in the middle */
    g_Render.RectFilled(this->rcSelectable, style.colComboBoxRect);

    /* Render outline */
    g_Render.Rect(this->rcSelectable, style.colSectionOutl);


    /* Fill the part of slider before the represented value */
    g_Render.RectFilled({ this->GetZeroPos(), this->rcSelectable.top + 1, this->iButtonPosX, this->rcSelectable.bottom - 1 },
                        style.colMenuStyle);

    /* Represented position of the value & its outline */
    g_Render.RectFilled(this->iButtonPosX - 1, this->rcSelectable.top - 1, this->iButtonPosX + 1, this->rcSelectable.bottom + 1, Color::White());
    g_Render.RectFilled(this->iButtonPosX + 1, this->rcSelectable.top - 1, this->iButtonPosX + 1, this->rcSelectable.bottom + 1, Color::Grey());
}


template <typename T>
void Slider<T>::SetupPositions()
{
    this->SetupBaseSize();

    this->rcSelectable =
    {
        rcBoundingBox.left,
        rcBoundingBox.top + pFont->iHeight,
        rcBoundingBox.right,
        rcBoundingBox.bottom
    };

    /* Update button position */
    this->iButtonPosX = (this->rcSelectable.left +
        static_cast<int>((*this->nValue - this->nMin) * float(this->rcBoundingBox.Width()) / (this->nMax - this->nMin)));
}


template <typename T>
bool Slider<T>::HandleMouseInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
    {
        /* Make sure we correct our hover state */
        this->bIsHovered = mouseCursor->IsInBounds(this->rcSelectable);

        if (uMsg != WM_LBUTTONDOWN)
            this->iDragX = mouseCursor->GetPos().x;
    }
    case WM_LBUTTONDOWN:
    {
        /* Make sure we correct our hover state */
        this->bIsHovered = mouseCursor->IsInBounds(this->rcSelectable);

        if (mouseCursor->bLMBPressed && bIsHovered)
            bPressed = true;
        else if (!mouseCursor->bLMBHeld && bPressed)
            bPressed = false;


        if (this->bPressed)
        {
            if (iDragX == 0)
                this->iDragX = mouseCursor->GetPos().x;

            this->iDragOffset = this->iDragX - this->iButtonPosX;
            this->iDragX = mouseCursor->GetPos().x;

            if (this->iDragOffset != 0)
            {
                this->RequestFocus();
                this->SetValue(static_cast<T>((this->iDragOffset * this->GetValuePerPixel()) + *this->nValue));
                return true;
            }
        }
        break;

    }
    }

    return false;
}


template <typename T>
bool Slider<T>::HandleKeyboardInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_HOME:
            this->SetValue(nMin);
            return true;
        case VK_END:
            this->SetValue(nMax);
        case VK_LEFT:
        case VK_DOWN:
            this->SetValue(*this->nValue - static_cast<T>(this->GetValuePerPixel()));
            return true;
        case VK_RIGHT:
        case VK_UP:
            this->SetValue(*this->nValue + static_cast<T>(this->GetValuePerPixel()));
            return true;
        case VK_NEXT:
            this->SetValue(*this->nValue - static_cast<T>(10.f * this->GetValuePerPixel()));
            return true;
        case VK_PRIOR:
            this->SetValue(*this->nValue + static_cast<T>(10.f * this->GetValuePerPixel()));
            return true;
        }
    }
    }

    return false;
}


template<typename T>
float Slider<T>::GetValuePerPixel() const
{
    return float(this->nMax - this->nMin) / this->szSizeObject.x;
}


template<typename T>
void Slider<T>::SetValue(T flValue)
{
    flValue = max(flValue, this->nMin);
    flValue = min(flValue, this->nMax);

    *this->nValue = flValue;
    this->SetupPositions();
}


template <typename T>
int Slider<T>::GetZeroPos()
{
    if (this->nMin < 0)
        return this->rcSelectable.left + static_cast<int>((0 - this->nMin) * float(this->rcBoundingBox.Width()) / float(this->nMax - this->nMin));

    return this->rcSelectable.left + 1;
}