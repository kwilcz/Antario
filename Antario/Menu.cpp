#include "Menu.h"
#include "Settings.h"
#include "SDK\IVEngineClient.h"

// Defined to avoid including windowsx.h
#define GET_X_LPARAM(lp)                        ((float)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((float)(short)HIWORD(lp))

// Init our statics
std::unique_ptr<MouseCursor> MenuMain::mouseCursor;
MenuStyle                    MenuMain::style;
CD3DFont*                    MenuMain::pFont        = nullptr;
bool                         BaseWindow::bIsDragged = false;


void Detach() { g_Settings.bCheatActive = false; }


void MouseCursor::Render()
{
    const auto x = this->vecPointPos.x;
    const auto y = this->vecPointPos.y;

    // Draw inner fill color
    Vector2D vecPos1 = { x + 1,  y + 1 };
    Vector2D vecPos2 = { x + 25, y + 12 };
    Vector2D vecPos3 = { x + 12, y + 25 };
    g_Render.TriangleFilled(vecPos1, vecPos2, vecPos3, g_Settings.colCursor);

    // Draw second smaller inner fill color
    vecPos1 = { x + 6,  y + 6 };
    vecPos2 = { x + 19, y + 12 };
    vecPos3 = { x + 12, y + 19 };
    g_Render.TriangleFilled(vecPos1, vecPos2, vecPos3, g_Settings.colCursor);

    // Draw border
    g_Render.Triangle(Vector2D(x, y), Vector2D(x + 25, y + 12), Vector2D(x + 12, y + 25), Color(0, 0, 0, 200));
}

void MouseCursor::RunThink(const UINT uMsg, const LPARAM lParam)
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
        default: break;
    }
}

bool MouseCursor::IsInBounds(const Vector2D vecDst1, const Vector2D vecDst2)
{
    return vecPointPos.x > vecDst1.x && vecPointPos.x < vecDst2.x && vecPointPos.y > vecDst1.y && vecPointPos.y < vecDst2.y;
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
    /*  Render all children
     *  Reverse iteration for dropdowns so they are on top */
    for (int it = this->vecChildren.size() - 1; it >= 0; --it)
        this->vecChildren.at(it)->Render();
}

void MenuMain::RunThink(const UINT uMsg, const LPARAM lParam)
{
    this->mouseCursor->RunThink(uMsg, lParam);
///TODO: Capture keyboard input, mainly for bind system
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
    this->AddChild(std::make_shared<DummySpace>(Vector2D(this->GetMaxChildWidth(), this->pFont->flHeight + this->style.iPaddingY)));
}

void MenuMain::AddCheckBox(std::string strSelectableLabel, bool* bValue)
{
    this->AddChild(std::make_shared<Checkbox>(strSelectableLabel, bValue, this));
}

void MenuMain::AddButton(std::string strSelectableLabel, void (&fnPointer)(), Vector2D vecButtonSize)
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



BaseWindow::BaseWindow(Vector2D vecPosition, Vector2D vecSize, CD3DFont* pUsedFont, CD3DFont* pHeaderFont, std::string strLabel)
{
    this->pFont         = pUsedFont;
    this->pHeaderFont   = pHeaderFont;
    this->strLabel      = strLabel;
    this->vecSize       = vecSize;

    this->iHeaderHeight = this->BaseWindow::GetHeaderHeight();
    this->MenuMain::SetPos(vecPosition);
    this->type = MenuSelectableType::TYPE_WINDOW;
}

void BaseWindow::Render()
{
    // Draw main background rectangle
    g_Render.RectFilledGradient(this->vecPosition, this->vecPosition + this->vecSize, Color(50, 50, 50, 255),
                                Color(20, 20, 20, 235), GradientType::GRADIENT_VERTICAL);

    // Draw header rect.
    g_Render.RectFilledGradient(this->vecPosition,
                                Vector2D(this->vecPosition.x + this->vecSize.x,
                                         this->vecPosition.y + this->iHeaderHeight), Color(50, 50, 50, 230),
                                Color(35, 35, 35, 230), GradientType::GRADIENT_VERTICAL);

    // Draw header string, defined as label.
    g_Render.String(this->vecPosition.x + (this->vecSize.x * 0.5f), this->vecPosition.y + (this->iHeaderHeight * 0.5f), CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y,
                    this->style.colHeaderText, this->pHeaderFont, this->strLabel.c_str());

    // Render all childrens
    MenuMain::Render();
}

bool BaseWindow::UpdateData()
{
    static auto bIsInitialized = false;

    const auto setChildPos = [&]()    // Set the position of all child sections
    {
        float flBiggestWidth = 0.f;
        float flUsedArea     = static_cast<float>(this->iHeaderHeight);
        float flPosX         = this->GetPos().x + this->style.iPaddingX;
        float flPosY         = 0.f;

        for (auto& it : this->vecChildren)
        {
            flPosY = this->GetPos().y + flUsedArea + this->style.iPaddingY;

            if (flPosY + it->GetSize().y > this->GetPos().y + this->GetSize().y)
            {
                flPosY -= flUsedArea;
                flPosY += static_cast<float>(this->iHeaderHeight);
                flUsedArea = 0.f;
                flPosX += flBiggestWidth + this->style.iPaddingX;
            }

            it->SetPos(Vector2D(flPosX, flPosY));
            flUsedArea += it->GetSize().y + this->style.iPaddingY;

            if (it->GetSize().x > flBiggestWidth)
                flBiggestWidth = it->GetSize().x;
        }
    };

    if (!bIsInitialized)
        setChildPos();
    
    /* Area where dragging windows is active */
    const Vector2D vecHeaderBounds = { this->vecPosition.x + this->vecSize.x,
                                       this->vecPosition.y + this->iHeaderHeight };

    // Check if mouse has been pressed in the proper area. If yes, save window state as dragged.
    if (this->mouseCursor->bLMBPressed && MenuMain::mouseCursor->IsInBounds(this->vecPosition, vecHeaderBounds))
        this->bIsDragged = true;
    else 
    if (!this->mouseCursor->bLMBPressed)
        this->bIsDragged = false;


    // Check if the window is dragged. If it is, move window by the cursor difference between ticks.
    static Vector2D vecOldMousePos = this->mouseCursor->vecPointPos;
    if (this->bIsDragged)
    {
        this->SetPos(this->vecPosition + (this->mouseCursor->vecPointPos - vecOldMousePos));
        vecOldMousePos = this->mouseCursor->vecPointPos;
        setChildPos();
    }
    else
        vecOldMousePos = this->mouseCursor->vecPointPos;
    
    // Call the inherited "UpdateData" function from the MenuMain class to loop through childs
    return MenuMain::UpdateData();
}

int BaseWindow::GetHeaderHeight()
{
    return static_cast<int>(this->pFont->flHeight + 2.f);
}



BaseSection::BaseSection(Vector2D vecSize, int iNumRows, std::string strLabel)
{
    this->vecSize         = vecSize;
    this->iNumRows        = iNumRows;
    this->strLabel        = strLabel;
    this->flMaxChildWidth = vecSize.x / iNumRows - 2 * this->style.iPaddingX;
    this->type            = MenuSelectableType::TYPE_SECTION;
}

void BaseSection::Render()
{
    g_Render.Rect(this->vecPosition, this->vecPosition + this->vecSize, this->style.colSectionOutl);
    g_Render.RectFilled(this->vecPosition, this->vecPosition + this->vecSize, this->style.colSectionFill);

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

    float flUsedArea    = 0.f;                  /* Specifies used rows in our menu window */
    float flColumnShift = 0.f;                  /* Specifies which column we draw in by shifting drawing "cursor" */
    int   iLeftRows     = this->iNumRows - 1;   /* Rows we have left to draw in */
    
    for (std::size_t it = 0; it < this->vecChildren.size(); it++)
    {
        const float flPosX = this->vecPosition.x + this->style.iPaddingX + flColumnShift;
        const float flPosY = this->vecPosition.y + flUsedArea + this->style.iPaddingY;

        /* Check if we will exceed bounds of the section */
        if ((flPosY + this->vecChildren.at(it)->GetSize().y) > (this->GetPos().y + this->GetSize().y))
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

        this->vecChildren.at(it)->SetPos(Vector2D(flPosX, flPosY));

        flUsedArea += this->vecChildren.at(it)->GetSize().y + this->style.iPaddingY;
    }

    this->bIsInitialized = true;   
}



Checkbox::Checkbox(std::string strLabel, bool *bValue, MenuMain* pParent)
{
    this->pParent        = pParent;
    this->strLabel       = strLabel;
    this->bCheckboxValue = bValue;
    this->bIsHovered     = false;

    this->vecSize           = { 100, this->pFont->flHeight };
    this->vecSelectableSize = { std::roundf(this->pFont->flHeight * 0.70f), std::roundf(this->pFont->flHeight * 0.70f) };
    this->type              = MenuSelectableType::TYPE_CHECKBOX;
}

void Checkbox::Render()
{
    /* Fill the inside of the button depending on activation */
    if (*this->bCheckboxValue)
    {
        g_Render.RectFilledGradient(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize,
                                    this->style.colCheckbox1, this->style.colCheckbox2,
                                    GradientType::GRADIENT_VERTICAL);
    }
    else
        g_Render.RectFilled(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize, this->style.colCheckbox1);

    /* Render the outline */
    g_Render.Rect(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize,
                  Color(15, 15, 15, 220));

    /* Render button label as its name */
    g_Render.String(this->vecSelectablePosition.x + this->vecSelectableSize.x + this->style.iPaddingX * 0.5f, this->vecPosition.y, 
                    CD3DFONT_DROPSHADOW, this->style.colText, this->pFont, this->strLabel.c_str());


    if (this->bIsHovered)
        g_Render.RectFilled(this->vecSelectablePosition + 1, this->vecSelectablePosition + this->vecSelectableSize, this->style.colHover);
}

bool Checkbox::UpdateData()
{
    static bool bIsChanged  = false;
    const float flVectorpos = (this->pFont->flHeight - this->vecSelectableSize.y) * 0.5f;

    /* Setup the position of our selectable area */
    this->vecSelectablePosition = this->vecPosition + Vector2D(flVectorpos, flVectorpos);

    if (this->mouseCursor->IsInBounds(this->vecPosition, (this->vecPosition + this->vecSize)))
    {
        if (this->bLMBPressedLast && !this->mouseCursor->bLMBPressed && !bIsChanged)
        {
            *this->bCheckboxValue = !*this->bCheckboxValue;
            bIsChanged            = true;
        }
        else 
        if (!this->mouseCursor->bLMBPressed && bIsChanged)
            bIsChanged = false;

        this->bIsHovered = true;
    }
    else
        this->bIsHovered = false;

    this->bLMBPressedLast = this->mouseCursor->bLMBPressed;
    return this->bIsHovered && bIsChanged;
}



Button::Button(std::string strLabel, void (&fnPointer)(), MenuMain* pParent, Vector2D vecButtonSize)
{
    this->pParent      = pParent;
    this->strLabel     = strLabel;
    this->fnActionPlay = fnPointer;
    this->bIsActivated = false;
    this->bIsHovered   = false;
    
    this->vecSize.x = vecButtonSize == Vector2D(0, 0) ? this->pParent->GetMaxChildWidth() : vecButtonSize.x;
    this->vecSize.y = this->pFont->flHeight + static_cast<float>(this->style.iPaddingY);
    this->type = MenuSelectableType::TYPE_BUTTON;
}

void Button::Render()
{
    /* Fill the body of the button */
    g_Render.RectFilledGradient(this->vecPosition, this->vecPosition + this->vecSize, this->style.colCheckbox1,
                                this->style.colCheckbox2, GradientType::GRADIENT_VERTICAL);
    /* Button outline */
    g_Render.Rect(this->vecPosition, this->vecPosition + this->vecSize, this->style.colSectionOutl);

    /* Text inside the button */
    g_Render.String(this->vecPosition.x + this->vecSize.x / 2.f, this->vecPosition.y + this->vecSize.y / 2.f,
                    CD3DFONT_DROPSHADOW | CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y, this->style.colText, this->pFont,
                    this->strLabel.c_str());


    if (this->bIsHovered)
        g_Render.RectFilled(this->vecPosition + 1, this->vecPosition + this->vecSize, this->style.colHover);
}

bool Button::UpdateData()
{
    if (this->mouseCursor->IsInBounds(this->vecPosition, (this->vecPosition + this->vecSize)))
    {
        if (this->bLMBPressedLast && !this->mouseCursor->bLMBPressed && !this->bIsActivated)
        {
            this->fnActionPlay(); /* Run the function passed as an arg. */
            this->bIsActivated = true;
        }
        else 
        if (!this->mouseCursor->bLMBPressed && bIsActivated)
            this->bIsActivated = false;

        this->bIsHovered = true;
    }
    else
        this->bIsHovered = false;

    this->bLMBPressedLast = this->mouseCursor->bLMBPressed;
    return this->bIsActivated;
}



ComboBox::ComboBox(std::string strLabel, std::vector<std::string> vecBoxOptions, int* iCurrentValue, MenuMain* pParent)
{
    this->bIsActive      = false;
    this->pParent        = pParent;
    this->strLabel       = strLabel;
    this->vecSelectables = vecBoxOptions;
    this->iCurrentValue  = iCurrentValue;
    this->bIsHovered     = false;
    this->bIsButtonHeld  = false;
    this->idHovered      = -2;

    this->vecSize.x         = this->pParent->GetMaxChildWidth();
    this->vecSize.y         = (this->pFont->flHeight + static_cast<float>(this->style.iPaddingY) * 0.5f) * 2.f;
    this->vecSelectableSize = { this->vecSize.x, this->pFont->flHeight + (static_cast<float>(this->style.iPaddingY) * 0.5f) };
    this->type              = MenuSelectableType::TYPE_COMBO;
}

void ComboBox::Render()
{
    /* Render the label (name) above the combo */
    g_Render.String(this->vecPosition, CD3DFONT_DROPSHADOW, this->style.colText, this->pFont, this->strLabel.c_str());

    /* Render the selectable with the value in the middle */
    g_Render.RectFilled(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize, this->style.colComboBoxRect);
    g_Render.String(this->vecSelectablePosition + (this->vecSelectableSize * 0.5f), CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y | CD3DFONT_DROPSHADOW,
        this->style.colText, this->pFont, this->vecSelectables.at(*this->iCurrentValue).c_str());

    /* Render the small triangle */
    [this]()
    {
        Vector2D vecPosMid, vecPosLeft, vecPosRight;
        Vector2D vecRightBottCorner = this->vecSelectablePosition + this->vecSelectableSize;

        vecPosMid.x   = vecRightBottCorner.x - 10.f;
        vecPosRight.x = vecRightBottCorner.x - 4.f;
        vecPosLeft.x  = vecRightBottCorner.x - 16.f;

        /* Draw two different versions (top-down, down-top) depending on activation */
        if (!this->bIsActive)
        {
            vecPosRight.y = vecPosLeft.y = this->vecSelectablePosition.y + 4.f;
            vecPosMid.y   = vecRightBottCorner.y - 4.f;
        }
        else
        {
            vecPosRight.y = vecPosLeft.y = vecRightBottCorner.y - 4.f;
            vecPosMid.y   = this->vecSelectablePosition.y + 4.f;
        }

        g_Render.TriangleFilled(vecPosLeft, vecPosRight, vecPosMid, this->style.colComboBoxRect * 0.5f);
        g_Render.Triangle(vecPosLeft, vecPosRight, vecPosMid, this->style.colSectionOutl);
    }();

    /* Highlight combo if hovered */
    if (this->bIsHovered)
        g_Render.RectFilled(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize, Color(100, 100, 100, 50));

    g_Render.Rect(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize, this->style.colSectionOutl);


    if (this->bIsActive)
    {
        /* Background square for the list */
        g_Render.RectFilledGradient(Vector2D(this->vecSelectablePosition.x, this->vecSelectablePosition.y + this->vecSelectableSize.y),
                                    Vector2D(this->vecSelectablePosition.x + this->vecSelectableSize.x,
                                             this->vecSelectablePosition.y + this->vecSelectableSize.y * (this->vecSelectables.size() + 1)),
                                    Color(40, 40, 40), Color(30, 30, 30), GRADIENT_VERTICAL);

        const auto vecMid = this->vecSelectablePosition + (this->vecSelectableSize * 0.5f);

        for (std::size_t it = 0; it < this->vecSelectables.size(); ++it)
            g_Render.String(Vector2D(vecMid.x, vecMid.y + this->vecSelectableSize.y * (it + 1)),
                            CD3DFONT_CENTERED_X | CD3DFONT_CENTERED_Y | CD3DFONT_DROPSHADOW,
                            this->style.colText, this->pFont, this->vecSelectables.at(it).c_str());

        if (this->idHovered != -1)
        {
            /* Highlights hovered position */
            const Vector2D vecElementPos = { this->vecSelectablePosition.x, this->vecSelectablePosition.y + this->vecSelectableSize.y * (this->idHovered + 1) };
            g_Render.RectFilled(vecElementPos, vecElementPos + this->vecSelectableSize, Color(100, 100, 100, 50));
        }
    }
}


bool ComboBox::UpdateData()
{
    this->vecSelectablePosition = Vector2D(this->vecPosition.x, this->vecPosition.y + this->pFont->flHeight + this->style.iPaddingY * 0.5f);

    if (mouseCursor->IsInBounds(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize))
    {
        if (this->mouseCursor->bLMBPressed && !this->bIsButtonHeld)
        {
            this->bIsActive     = !bIsActive;
            this->bIsButtonHeld = true;
            return true;
        }
        else if (!this->mouseCursor->bLMBPressed && this->bIsButtonHeld)
            this->bIsButtonHeld = false;

        this->bIsHovered = true;
    }
    else
        this->bIsHovered = false;

    if (this->bIsActive)
    {
        if (this->mouseCursor->IsInBounds(Vector2D(this->vecSelectablePosition.x, this->vecSelectablePosition.y + this->vecSelectableSize.y),
                                          Vector2D(this->vecSelectablePosition.x + this->vecSelectableSize.x,
                                                   this->vecSelectablePosition.y + this->vecSelectableSize.y * (this->vecSelectables.size() + 1))))
        {
            for (std::size_t it = 0; it < this->vecSelectables.size(); ++it)
            {
                const auto vecElementPos = Vector2D(this->vecSelectablePosition.x, this->vecSelectablePosition.y + this->vecSelectableSize.y * (it + 1));

                if (this->mouseCursor->IsInBounds(vecElementPos, Vector2D(vecElementPos.x + this->vecSelectableSize.x,
                                                                          vecElementPos.y + this->vecSelectableSize.y)))
                {
                    this->idHovered = it;

                    if (this->mouseCursor->bLMBPressed && !this->bIsButtonHeld)
                    {
                        this->idHovered = it;

                        if (this->mouseCursor->bLMBPressed && !this->bIsButtonHeld)
                        {
                            *this->iCurrentValue = it;
                            this->bIsButtonHeld  = true;
                            this->idHovered      = -1;
                            this->bIsActive      = false;
                        }
                    }
                    else if (!this->mouseCursor->bLMBPressed && this->bIsButtonHeld)
                        this->bIsButtonHeld = false;
                }
            }
        }
        else
            this->idHovered = -1;
    }

    if (!this->mouseCursor->bLMBPressed && this->bIsButtonHeld)
        this->bIsButtonHeld = false;

    if (!g_Settings.bMenuOpened)
        this->bIsActive = false;

    return this->idHovered >= 0 || (this->bIsButtonHeld && this->idHovered == -1);
}

Vector2D ComboBox::GetSelectableSize()
{
    Vector2D vecTmpSize;
    vecTmpSize.y = this->pFont->flHeight + static_cast<float>(this->style.iPaddingY) * 0.5f;
    vecTmpSize.x = this->GetSize().x;
    return vecTmpSize;
}


template<typename T>
inline Slider<T>::Slider(const std::string& strLabel, T* flValue, T flMinValue, T flMaxValue, MenuMain* pParent)
{
    this->pParent      = pParent;
    this->strLabel     = strLabel;
    this->nValue      = flValue;
    this->nMin        = flMinValue;
    this->nMax        = flMaxValue;

    this->vecSize.x         = this->pParent->GetMaxChildWidth();
    this->vecSize.y         = (this->pFont->flHeight + static_cast<float>(this->style.iPaddingY) * 0.5f) * 2.f;
    this->vecSelectableSize = { this->vecSize.x, this->pFont->flHeight + (static_cast<float>(this->style.iPaddingY) * 0.5f) };
    this->type              = MenuSelectableType::TYPE_SLIDER;
}


template<typename T>
void Slider<T>::Render()
{
    std::stringstream ssToRender;
    ssToRender << strLabel << ": " << *this->nValue;

    /* Render the label (name) above the combo */
    g_Render.String(this->vecPosition, CD3DFONT_DROPSHADOW, this->style.colText, this->pFont, ssToRender.str().c_str());

    /* Render the selectable with the value in the middle */
    g_Render.RectFilled(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize, this->style.colComboBoxRect);

    /* Render outline */
    g_Render.Rect(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize, this->style.colSectionOutl);

    /* Represented position of the value & its outline */
    g_Render.RectFilled(Vector2D(this->flButtonPosX - 1, this->vecSelectablePosition.y),
                        Vector2D(this->flButtonPosX + 1, this->vecSelectablePosition.y + this->vecSelectableSize.y),
                        Color::White());
    g_Render.Rect(Vector2D(this->flButtonPosX - 2, this->vecSelectablePosition.y),
                  Vector2D(this->flButtonPosX + 1, this->vecSelectablePosition.y + this->vecSelectableSize.y), Color::Black());

    /* Fill the part of slider before the represented value */
    if (this->flButtonPosX - 2 > this->vecSelectablePosition.x + 1)
        g_Render.RectFilledGradient(this->vecSelectablePosition + 1,
                                    Vector2D(this->flButtonPosX - 2, this->vecSelectablePosition.y + this->vecSelectableSize.y),
                                    Color(200, 0, 100), Color(255, 0, 100), GradientType::GRADIENT_HORIZONTAL);
    ///TODO: Make colors not hardcoded + smaller slider.
}


template<typename T>
bool Slider<T>::UpdateData()
{
    this->vecSelectablePosition = Vector2D(this->vecPosition.x, this->vecPosition.y + this->pFont->flHeight + this->style.iPaddingY * 0.5f);
    this->flButtonPosX          = this->vecSelectablePosition.x + ((*this->nValue - this->nMin) * this->vecSize.x / (this->nMax - this->nMin));

    if (mouseCursor->IsInBounds(this->vecSelectablePosition, this->vecSelectablePosition + this->vecSelectableSize))
    {
        this->bIsHovered = true;
        if (this->mouseCursor->bLMBPressed)
            this->bPressed = true;
    }
    if (!this->mouseCursor->bLMBPressed)
        this->bPressed = false;


    if (this->bPressed)
    {
        if (!flDragX)
            this->flDragX = this->mouseCursor->GetPosition().x;

        this->flDragOffset = this->flDragX - this->flButtonPosX;
        this->flDragX      = this->mouseCursor->GetPosition().x;

        this->SetValue(static_cast<T>((this->flDragOffset * this->GetValuePerPixel()) + *this->nValue));
        return true;
    }

    return false;
}


template<typename T>
float Slider<T>::GetValuePerPixel() const
{
    return static_cast<float>(this->nMax - this->nMin) / this->vecSize.x;
}


template<typename T>
void Slider<T>::SetValue(T flValue)
{
    flValue = max(flValue, this->nMin);
    flValue = min(flValue, this->nMax);

    *this->nValue = flValue;
}



void MenuMain::Initialize()
{
    static int testint;
    static int testint2;
    static float float123 = 10.f;
    static int testint3 = 2;
    /* Create our main window (Could have multiple if you'd create vec. for it) */
    std::shared_ptr<BaseWindow> mainWindow = std::make_shared<BaseWindow>(Vector2D(450, 450), Vector2D(360, 256), g_Fonts.pFontTahoma8.get(), g_Fonts.pFontTahoma10.get(), "Antario - Main"); 
    {
        std::shared_ptr<BaseSection> sectMain = std::make_shared<BaseSection>(Vector2D(310, 100), 2, "Test Section 1");
        {
            sectMain->AddCheckBox ("Bunnyhop Enabled", &g_Settings.bBhopEnabled);
            sectMain->AddCheckBox ("Show Player Names", &g_Settings.bShowNames);
            sectMain->AddButton   ("Shutdown", Detach);
            sectMain->AddSlider   ("TestSlider", &float123, 0, 20);
            sectMain->AddSlider   ("intslider", &testint3, 0, 10);

            sectMain->AddCombo    ("TestCombo", std::vector<std::string>{ "Value1", "Value2", "Value3" }, &testint);
        }
        mainWindow->AddChild(sectMain);
        std::shared_ptr<BaseSection> sectMain2 = std::make_shared<BaseSection>(Vector2D(310, 100), 2, "Test Section 2");
        {
            /* To be removed */
            sectMain2->AddCombo   ("TestCombo2", std::vector<std::string>{ "ttest", "ttest2", "ttest3" }, &testint2);
            sectMain2->AddCheckBox("CheckboxSect2_1", &g_Settings.bShowNames);
            sectMain2->AddCheckBox("CheckboxSect2_2", &g_Settings.bShowNames);
        }
        mainWindow->AddChild(sectMain2);
    }
    this->AddChild(mainWindow);

    this->mouseCursor = std::make_unique<MouseCursor>();    /* Create our mouse cursor (one instance only) */
}
