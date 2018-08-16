#include "GUI.h"

// Defined to avoid including windowsx.h
#define GET_X_LPARAM(lp)                        ((float)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((float)(short)HIWORD(lp))

// Init our statics
std::unique_ptr<MouseCursor> MenuMain::mouseCursor;
MenuStyle                    MenuMain::style;
CD3DFont*                    MenuMain::pFont = nullptr;
bool                         BaseWindow::bIsDragged = false;


void MouseCursor::Render()
{
    const auto x = this->vecPointPos.x;
    const auto y = this->vecPointPos.y;

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
    return rcRect.ContainsPoint(this->GetPosition());
}


void MenuMain::SetParent(MenuMain* newParent)
{
    this->pParent = newParent;
}

void MenuMain::AddChild(const std::shared_ptr<MenuMain>& child)
{
    this->vecChildren.push_back(child);
}

void MenuMain::Render()
{
    /* Render all children
    *  Reverse iteration for dropdowns so they are on top */
    for (int it = this->vecChildren.size() - 1; it >= 0; --it)
        this->vecChildren.at(it)->Render();
}

void MenuMain::RunThink(const UINT uMsg, const LPARAM lParam)
{
    mouseCursor->RunThink(uMsg, lParam);

    /* Suggestion: Run mouse input through all chlid objects with uMsg & lParam
    * Would save some performance (minor). I'm just too lazy to do it */
    this->UpdateData();
}

bool MenuMain::UpdateData()
{
    if (!this->vecChildren.empty())
    {
        for (auto& it : this->vecChildren)
            if (it->UpdateData())
                return true;        /* Return true if our updatedata did change something. */
    }
    return false;
}

void MenuMain::AddDummy()
{
    this->AddChild(std::make_shared<DummySpace>(SPoint(this->GetMaxChildWidth(), pFont->iHeight + style.iPaddingY)));
}

void MenuMain::AddCheckBox(std::string strSelectableLabel, bool* bValue)
{
    this->AddChild(std::make_shared<Checkbox>(strSelectableLabel, bValue, this));
}

void MenuMain::AddButton(std::string strSelectableLabel, void(&fnPointer)(), SPoint vecButtonSize)
{
    this->AddChild(std::make_shared<Button>(strSelectableLabel, fnPointer, this, vecButtonSize));
}

void MenuMain::AddCombo(std::string strSelectableLabel, std::vector<std::string> vecBoxOptions, int* iVecIndex)
{
    this->AddChild(std::make_shared<ComboBox>(strSelectableLabel, vecBoxOptions, iVecIndex, this));
}

void MenuMain::AddSlider(std::string strLabel, float* flValue, float flMinValue, float flMaxValue)
{
    this->AddChild(std::make_shared<Slider<float>>(strLabel, flValue, flMinValue, flMaxValue, this));
}

void MenuMain::AddSlider(std::string strLabel, int* iValue, int iMinValue, int iMaxValue)
{
    this->AddChild(std::make_shared<Slider<int>>(strLabel, iValue, iMinValue, iMaxValue, this));
}



BaseWindow::BaseWindow(SPoint ptPosition, SPoint szSize, CD3DFont* pUsedFont, CD3DFont* pHeaderFont, std::string strLabel)
{
    pFont = pUsedFont;
    this->pHeaderFont = pHeaderFont;
    this->strLabel = strLabel;
    this->szSize = szSize;

    this->iHeaderHeight = this->BaseWindow::GetHeaderHeight();
    this->MenuMain::SetPos(ptPosition);
    this->type = MenuSelectableType::TYPE_WINDOW;
}

void BaseWindow::Render()
{
    // Draw main background rectangle
    g_Render.RectFilledGradient(this->ptPosition, this->ptPosition + this->szSize, Color(50, 50, 50, 255),
        Color(20, 20, 20, 235), GradientType::GRADIENT_VERTICAL);

    // Draw header rect.
    g_Render.RectFilledGradient(this->ptPosition,
        SPoint(this->ptPosition.x + this->szSize.x,
            this->ptPosition.y + this->iHeaderHeight), Color(50, 50, 50, 230),
        Color(35, 35, 35, 230), GradientType::GRADIENT_VERTICAL);

    // Draw header string, defined as label.
    g_Render.String(this->ptPosition.x + (this->szSize.x * 0.5f), this->ptPosition.y + (this->iHeaderHeight * 0.5f), CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y,
        style.colHeaderText, this->pHeaderFont, this->strLabel.c_str());

    // Render all childrens
    MenuMain::Render();
}

bool BaseWindow::UpdateData()
{
    static auto bIsInitialized = false;

    const auto setChildPos = [&]()    // Set the position of all child sections
    {
        float flBiggestWidth = 0.f;
        float flUsedArea = float(this->iHeaderHeight);
        float flPosX = this->GetPos().x + style.iPaddingX;
        float flPosY = 0.f;

        for (auto& it : this->vecChildren)
        {
            flPosY = this->GetPos().y + flUsedArea + style.iPaddingY;

            if (flPosY + it->GetSize().y > this->GetPos().y + this->GetSize().y)
            {
                flPosY -= flUsedArea;
                flPosY += float(this->iHeaderHeight);
                flUsedArea = 0.f;
                flPosX += flBiggestWidth + style.iPaddingX;
            }

            it->SetPos(SPoint(flPosX, flPosY));
            flUsedArea += it->GetSize().y + style.iPaddingY;

            if (it->GetSize().x > flBiggestWidth)
                flBiggestWidth = it->GetSize().x;
        }
    };

    if (!bIsInitialized)
        setChildPos();

    /* Area where dragging windows is active */
    const SPoint vecHeaderBounds = { this->ptPosition.x + this->szSize.x,
        this->ptPosition.y + this->iHeaderHeight };

    // Check if mouse has been pressed in the proper area. If yes, save window state as dragged.
    if (mouseCursor->bLMBPressed && MenuMain::mouseCursor->IsInBounds(this->ptPosition, vecHeaderBounds))
        this->bIsDragged = true;
    else
        if (!mouseCursor->bLMBHeld)
            this->bIsDragged = false;


    // Check if the window is dragged. If it is, move window by the cursor difference between ticks.
    static SPoint vecOldMousePos = mouseCursor->vecPointPos;
    if (this->bIsDragged)
    {
        this->SetPos(this->ptPosition + (mouseCursor->vecPointPos - vecOldMousePos));
        vecOldMousePos = mouseCursor->vecPointPos;
        setChildPos();
    }
    else
        vecOldMousePos = mouseCursor->vecPointPos;

    // Call the inherited "UpdateData" function from the MenuMain class to loop through childs
    return MenuMain::UpdateData();
}

int BaseWindow::GetHeaderHeight()
{
    return int(pFont->iHeight + 2.f);
}



BaseSection::BaseSection(SPoint szSize, int iNumRows, std::string strLabel)
{
    this->szSize = szSize;
    this->iNumRows = iNumRows;
    this->strLabel = strLabel;
    this->flMaxChildWidth = szSize.x / iNumRows - 2 * style.iPaddingX;
    this->type = MenuSelectableType::TYPE_SECTION;
}

void BaseSection::Render()
{
    g_Render.Rect(this->ptPosition, this->ptPosition + this->szSize, style.colSectionOutl);
    g_Render.RectFilled(this->ptPosition, this->ptPosition + this->szSize, style.colSectionFill);

    MenuMain::Render();
}

bool BaseSection::UpdateData()
{
    this->SetupPositions();
    return MenuMain::UpdateData();
}

void BaseSection::SetupPositions()
{
    if (!this->bIsDragged && this->bIsInitialized)
        return;

    float flUsedArea = 0.f;                  /* Specifies used rows in our menu window */
    float flColumnShift = 0.f;                  /* Specifies which column we draw in by shifting drawing "cursor" */
    int   iLeftRows = this->iNumRows - 1;   /* Rows we have left to draw in */

    for (std::size_t it = 0; it < this->vecChildren.size(); it++)
    {
        const float flPosX = this->ptPosition.x + style.iPaddingX + flColumnShift;
        const float flPosY = this->ptPosition.y + flUsedArea + style.iPaddingY;

        /* Check if we will exceed bounds of the section */
        if ((flPosY + this->vecChildren.at(it)->GetSize().y) >(this->GetPos().y + this->GetSize().y))
        {
            /* Check if we have any left rows to draw in */
            if (iLeftRows > 0)
            {
                /* Shift our X position and run this loop instance once again */
                flColumnShift += this->GetSize().x / this->iNumRows;
                flUsedArea = 0.f;
                --iLeftRows;
                --it;
                continue;
            }
            else
                break;  /* Don't set up positions if there are too many selectables so its easy to spot an error as they will draw in top-left corner. */
        }

        this->vecChildren.at(it)->SetPos(SPoint(flPosX, flPosY));

        flUsedArea += this->vecChildren.at(it)->GetSize().y + style.iPaddingY;
    }

    this->bIsInitialized = true;
}



Checkbox::Checkbox(std::string strLabel, bool *bValue, MenuMain* pParent)
{
    this->pParent = pParent;
    this->strLabel = strLabel;
    this->bCheckboxValue = bValue;
    this->bIsHovered = false;

    this->szSize = { 100, pFont->iHeight };
    this->szSelectableSize = { int(std::roundf(pFont->iHeight * 0.70f)), int(std::roundf(pFont->iHeight * 0.70f)) };
    this->type = MenuSelectableType::TYPE_CHECKBOX;
}

void Checkbox::Render()
{
    /* Fill the inside of the button depending on activation */
    if (*this->bCheckboxValue)
    {
        g_Render.RectFilledGradient(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize,
            style.colCheckbox1, style.colCheckbox2,
            GradientType::GRADIENT_VERTICAL);
    }
    else
        g_Render.RectFilled(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize, style.colCheckbox1);

    /* Render the outline */
    g_Render.Rect(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize,
        Color(15, 15, 15, 220));

    /* Render button label as its name */
    g_Render.String(this->ptSelectablePosition.x + this->szSelectableSize.x + style.iPaddingX * 0.5f, this->ptPosition.y,
        CD3DFONT_DROPSHADOW, style.colText, pFont, this->strLabel.c_str());


    if (this->bIsHovered)
        g_Render.RectFilled(this->ptSelectablePosition + 1, this->ptSelectablePosition + this->szSelectableSize, style.colHover);
}

bool Checkbox::UpdateData()
{
    const float flVectorpos = (pFont->iHeight - this->szSelectableSize.y) * 0.5f;

    /* Setup the position of our selectable area */
    this->ptSelectablePosition = this->ptPosition + SPoint(flVectorpos, flVectorpos);

    if (mouseCursor->IsInBounds(this->ptPosition, (this->ptPosition + this->szSelectableSize)))
    {
        if (mouseCursor->bLMBPressed)
            *this->bCheckboxValue = !*this->bCheckboxValue;

        this->bIsHovered = true;
    }
    else
        this->bIsHovered = false;

    return this->bIsHovered && mouseCursor->bLMBPressed;
}



Button::Button(std::string strLabel, void(&fnPointer)(), MenuMain* pParent, SPoint vecButtonSize)
{
    this->pParent = pParent;
    this->strLabel = strLabel;
    this->fnActionPlay = fnPointer;
    this->bIsActivated = false;
    this->bIsHovered = false;

    this->szSize.x = vecButtonSize == SPoint(0, 0) ? this->pParent->GetMaxChildWidth() : vecButtonSize.x;
    this->szSize.y = pFont->iHeight + float(style.iPaddingY);
    this->type = MenuSelectableType::TYPE_BUTTON;
}

void Button::Render()
{
    /* Fill the body of the button */
    g_Render.RectFilledGradient(this->ptPosition, this->ptPosition + this->szSize, style.colCheckbox1,
        style.colCheckbox2, GradientType::GRADIENT_VERTICAL);
    /* Button outline */
    g_Render.Rect(this->ptPosition, this->ptPosition + this->szSize, style.colSectionOutl);

    /* Text inside the button */
    g_Render.String(this->ptPosition.x + this->szSize.x / 2.f, this->ptPosition.y + this->szSize.y / 2.f,
        CD3DFONT_DROPSHADOW | CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y, style.colText, pFont,
        this->strLabel.c_str());


    if (this->bIsHovered)
        g_Render.RectFilled(this->ptPosition + 1, this->ptPosition + this->szSize, style.colHover);
}

bool Button::UpdateData()
{
    if (mouseCursor->IsInBounds(this->ptPosition, (this->ptPosition + this->szSize)))
    {
        if (mouseCursor->bLMBPressed)
            this->fnActionPlay(); /* Run the function passed as an arg. */

        this->bIsHovered = true;
    }
    else
        this->bIsHovered = false;

    return this->bIsHovered && mouseCursor->bLMBPressed;
}



ComboBox::ComboBox(std::string strLabel, std::vector<std::string> vecBoxOptions, int* iCurrentValue, MenuMain* pParent)
{
    this->bIsActive = false;
    this->pParent = pParent;
    this->strLabel = strLabel;
    this->vecSelectables = vecBoxOptions;
    this->iCurrentValue = iCurrentValue;
    this->bIsHovered = false;
    this->bIsButtonHeld = false;
    this->idHovered = -1;

    this->szSize.x = this->pParent->GetMaxChildWidth();
    this->szSize.y = (pFont->iHeight + float(style.iPaddingY) * 0.5f) * 2.f;
    this->szSelectableSize = { this->szSize.x, pFont->iHeight + int(float(style.iPaddingY) * 0.5f) };
    this->type = MenuSelectableType::TYPE_COMBO;
}

void ComboBox::Render()
{
    /* Render the label (name) above the combo */
    g_Render.String(this->ptPosition, CD3DFONT_DROPSHADOW, style.colText, pFont, this->strLabel.c_str());

    /* Render the selectable with the value in the middle */
    g_Render.RectFilled(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize, style.colComboBoxRect);
    g_Render.String(this->ptSelectablePosition + (this->szSelectableSize * 0.5f),
        CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y | CD3DFONT_DROPSHADOW,
        style.colText, pFont, this->vecSelectables.at(*this->iCurrentValue).c_str());

    /* Render the small triangle */
    [this]()
    {
        SPoint ptPosMid, ptPosLeft, ptPosRight;
        SPoint vecRightBottCorner = this->ptSelectablePosition + this->szSelectableSize;

        ptPosMid.x = vecRightBottCorner.x - 10.f;
        ptPosRight.x = vecRightBottCorner.x - 4.f;
        ptPosLeft.x = vecRightBottCorner.x - 16.f;

        /* Draw two different versions (top-down, down-top) depending on activation */
        if (!this->bIsActive)
        {
            ptPosRight.y = ptPosLeft.y = this->ptSelectablePosition.y + 4.f;
            ptPosMid.y = vecRightBottCorner.y - 4.f;
        }
        else
        {
            ptPosRight.y = ptPosLeft.y = vecRightBottCorner.y - 4.f;
            ptPosMid.y = this->ptSelectablePosition.y + 4.f;
        }

        g_Render.TriangleFilled(ptPosLeft, ptPosRight, ptPosMid, style.colComboBoxRect * 0.5f);
        g_Render.Triangle(ptPosLeft, ptPosRight, ptPosMid, style.colSectionOutl);
    }();

    /* Highlight combo if hovered */
    if (this->bIsHovered)
        g_Render.RectFilled(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize, Color(100, 100, 100, 50));

    g_Render.Rect(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize, style.colSectionOutl);


    if (this->bIsActive)
    {
        /* Background square for the list */
        g_Render.RectFilledGradient(SPoint(this->ptSelectablePosition.x, this->ptSelectablePosition.y + this->szSelectableSize.y),
            SPoint(this->ptSelectablePosition.x + this->szSelectableSize.x,
                this->ptSelectablePosition.y + this->szSelectableSize.y * (this->vecSelectables.size() + 1)),
            Color(40, 40, 40), Color(30, 30, 30), GRADIENT_VERTICAL);

        const auto vecMid = this->ptSelectablePosition + (this->szSelectableSize * 0.5f);

        for (std::size_t it = 0; it < this->vecSelectables.size(); ++it)
            g_Render.String(SPoint(vecMid.x, vecMid.y + this->szSelectableSize.y * (it + 1)),
                CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y | CD3DFONT_DROPSHADOW,
                style.colText, pFont, this->vecSelectables.at(it).c_str());

        if (this->idHovered != -1)
        {
            /* Highlights hovered position */
            const SPoint vecElementPos = { this->ptSelectablePosition.x, this->ptSelectablePosition.y + this->szSelectableSize.y * (this->idHovered + 1) };
            g_Render.RectFilled(vecElementPos, vecElementPos + this->szSelectableSize, Color(100, 100, 100, 50));
        }
    }
}


bool ComboBox::UpdateData()
{
    this->ptSelectablePosition = SPoint(this->ptPosition.x, this->ptPosition.y + pFont->iHeight + style.iPaddingY * 0.5f);

    if (mouseCursor->IsInBounds(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize))
    {
        this->bIsHovered = true;

        if (mouseCursor->bLMBPressed)
        {
            this->bIsActive = !bIsActive;
            return true;
        }
    }
    else
    {
        this->bIsHovered = false;

        if (this->bIsActive)
        {
            if (mouseCursor->IsInBounds(SPoint(this->ptSelectablePosition.x, this->ptSelectablePosition.y + this->szSelectableSize.y),
                                        SPoint(this->ptSelectablePosition.x + this->szSelectableSize.x,
                                               this->ptSelectablePosition.y + this->szSelectableSize.y * (this->vecSelectables.size() + 1))))
            {
                for (std::size_t it = 0; it < this->vecSelectables.size(); ++it)
                {
                    const auto ptElementPos = SPoint(this->ptSelectablePosition.x, this->ptSelectablePosition.y + this->szSelectableSize.y * (it + 1));

                    if (mouseCursor->IsInBounds(ptElementPos, { ptElementPos.x + this->szSelectableSize.x, ptElementPos.y + this->szSelectableSize.y + 1 }))
                    {
                        this->idHovered = it;
                        if (mouseCursor->bLMBPressed)
                        {
                            *this->iCurrentValue = it;
                            this->idHovered = -1;
                            this->bIsActive = false;
                            return true;
                        }
                    }
                }
            }
            else
            {
                this->idHovered = -1;
                if (mouseCursor->bLMBPressed)
                    this->bIsActive = false;
            }
        }
    }

    return false;
}


SPoint ComboBox::GetSelectableSize()
{
    SPoint vecTmpSize;
    vecTmpSize.y = pFont->iHeight + float(style.iPaddingY) * 0.5f;
    vecTmpSize.x = this->GetSize().x;
    return vecTmpSize;
}


template<typename T>
Slider<T>::Slider(const std::string& strLabel, T* flValue, T flMinValue, T flMaxValue, MenuMain* pParent)
{
    this->pParent = pParent;
    this->strLabel = strLabel;
    this->nValue = flValue;
    this->nMin = flMinValue;
    this->nMax = flMaxValue;

    this->szSize.x = this->pParent->GetMaxChildWidth();
    this->szSize.y = (pFont->iHeight + float(style.iPaddingY) * 0.5f) * 2.f;
    this->szSelectableSize = { this->szSize.x, pFont->iHeight + int(float(style.iPaddingY) * 0.5f) };
    this->type = MenuSelectableType::TYPE_SLIDER;
}


template<typename T>
void Slider<T>::Render()
{
    std::stringstream ssToRender;
    ssToRender << strLabel << ": " << *this->nValue;

    /* Render the label (name) above the combo */
    g_Render.String(this->ptPosition, CD3DFONT_DROPSHADOW, style.colText, pFont, ssToRender.str().c_str());

    /* Render the selectable with the value in the middle */
    g_Render.RectFilled(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize, style.colComboBoxRect);

    /* Render outline */
    g_Render.Rect(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize, style.colSectionOutl);

    /* Represented position of the value & its outline */
    g_Render.RectFilled(SPoint(this->iButtonPosX - 1, this->ptSelectablePosition.y),
        SPoint(this->iButtonPosX + 1, this->ptSelectablePosition.y + this->szSelectableSize.y),
        Color::White());
    g_Render.Rect(SPoint(this->iButtonPosX - 2, this->ptSelectablePosition.y),
        SPoint(this->iButtonPosX + 1, this->ptSelectablePosition.y + this->szSelectableSize.y), Color::Black());

    /* Fill the part of slider before the represented value */
    if (this->iButtonPosX - 2 > this->ptSelectablePosition.x + 1)
        g_Render.RectFilledGradient(this->ptSelectablePosition + 1,
            SPoint(this->iButtonPosX - 2, this->ptSelectablePosition.y + this->szSelectableSize.y),
            Color(200, 0, 100), Color(255, 0, 100), GradientType::GRADIENT_HORIZONTAL);
    ///TODO: Make colors not hardcoded + smaller slider.
}


template<typename T>
bool Slider<T>::UpdateData()
{
    this->ptSelectablePosition = SPoint(this->ptPosition.x, this->ptPosition.y + pFont->iHeight + style.iPaddingY * 0.5f);
    this->iButtonPosX = this->ptSelectablePosition.x + ((*this->nValue - this->nMin) * this->szSize.x / (this->nMax - this->nMin));

    if (mouseCursor->IsInBounds(this->ptSelectablePosition, this->ptSelectablePosition + this->szSelectableSize))
    {
        this->bIsHovered = true;
        if (mouseCursor->bLMBPressed)
            this->bPressed = true;
    }
    if (!mouseCursor->bLMBHeld)
        this->bPressed = false;


    if (this->bPressed)
    {
        if (!iDragX)
            this->iDragX = mouseCursor->GetPosition().x;

        this->iDragOffset = this->iDragX - this->iButtonPosX;
        this->iDragX = mouseCursor->GetPosition().x;

        this->SetValue(static_cast<T>((this->iDragOffset * this->GetValuePerPixel()) + *this->nValue));
        return true;
    }

    return false;
}


template<typename T>
float Slider<T>::GetValuePerPixel() const
{
    return float(this->nMax - this->nMin) / this->szSize.x;
}


template<typename T>
void Slider<T>::SetValue(T flValue)
{
    flValue = max(flValue, this->nMin);
    flValue = min(flValue, this->nMax);

    *this->nValue = flValue;
}