#include "GUI\GUI.h"
#include "Settings.h"


void Detach() { g_Settings.bCheatActive = false; }


void MenuMain::Initialize()
{
    static int testint;
    static int testint2;
    static float float123 = 10.f;
    static int testint3 = 2;
    /* Create our main window (Could have multiple if you'd create vec. for it) */
    auto mainWindow = std::make_shared<BaseWindow>(SPoint(450, 450), SPoint(360, 256), g_Fonts.pFontTahoma8.get(), g_Fonts.pFontTahoma10.get(), "Antario - Main");
    {
        auto sectMain = std::make_shared<BaseSection>(SPoint(310, 100), 2, "Test Section 1");
        {
            sectMain->AddCheckBox("Bunnyhop Enabled", &g_Settings.bBhopEnabled);
            sectMain->AddCheckBox("Show Player Names", &g_Settings.bShowNames);
            sectMain->AddButton("Shutdown", Detach);
            sectMain->AddSlider("TestSlider", &float123, 0, 20);
            sectMain->AddSlider("intslider", &testint3, 0, 10);
            sectMain->AddCombo("TestCombo", std::vector<std::string>{ "Value1", "Value2", "Value3" }, &testint);
        }
        mainWindow->AddChild(sectMain);
        auto sectMain2 = std::make_shared<BaseSection>(SPoint(310, 100), 2, "Test Section 2");
        {
            /* To be removed */
            sectMain2->AddCombo("TestCombo2", std::vector<std::string>{ "ttest", "ttest2", "ttest3" }, &testint2);
            sectMain2->AddCheckBox("CheckboxSect2_1", &g_Settings.bShowNames);
            sectMain2->AddCheckBox("CheckboxSect2_2", &g_Settings.bShowNames);
        }
        mainWindow->AddChild(sectMain2);
    }
    this->AddChild(mainWindow);

    mouseCursor = std::make_unique<MouseCursor>();    /* Create our mouse cursor (one instance only) */
}
