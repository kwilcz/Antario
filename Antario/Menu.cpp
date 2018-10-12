#include "GUI\GUI.h"
#include "Settings.h"


void Detach() { g_Settings.bCheatActive = false; }


void MenuMain::Initialize()
{
    static int   testint;
    static int   testint2;
    static int   testint3 = 2;
    static float float123 = 10.f;
    /* Create our main window (Could have multiple if you'd create vec. for it) */
    auto mainWindow = std::make_shared<Window>("Antario - Main", SSize(360, 256), g_Fonts.pFontTahoma8, g_Fonts.pFontTahoma10);
    {
        ///TODO: window->AddTab()
        auto tab1 = std::make_shared<Tab>("Main Tab", 2, mainWindow);
        {
            /* Create sections for it */
            auto sectMain = tab1->AddSection("TestSect", 1.f);
            {
                /* Add controls within section */
                sectMain->AddCheckBox("Bunnyhop Enabled", &g_Settings.bBhopEnabled);
                sectMain->AddCheckBox("Show Player Names", &g_Settings.bShowNames);
                sectMain->AddButton("Shutdown", Detach);
                sectMain->AddSlider("TestSlider", &float123, 0, 20);
                sectMain->AddSlider("intslider", &testint3, 0, 10);
                //sectMain->AddCombo("TestCombo", std::vector<std::string>{ "Value1", "Value2", "Value3" }, &testint);
            }

            auto sectMain2 = tab1->AddSection("TestSect2", 1.f);
            {
                //sectMain2->AddCombo("TestCombo2", std::vector<std::string>{ "ttest", "ttest2", "ttest3" }, &testint2);
                sectMain2->AddCheckBox("CheckboxSect2_1", &g_Settings.bShowBoxes);
                sectMain2->AddCheckBox("Show Player Boxes", &g_Settings.bShowBoxes);
                sectMain2->AddCheckBox("Show Player Weapons", &g_Settings.bShowWeapons);
            }
        } mainWindow->AddChild(tab1);   /* For now */

        auto tab2 = std::make_shared<Tab>("Test Tab", 1, mainWindow);
        {
            auto sectMain = tab2->AddSection("TestSect", .5f);
            {
                /* Add controls within section */
                sectMain->AddCheckBox("CheckboxSect2_1", &g_Settings.bShowBoxes);
                sectMain->AddCheckBox("Show Player Boxes", &g_Settings.bShowBoxes);
                sectMain->AddCheckBox("Show Player Weapons", &g_Settings.bShowWeapons);
                sectMain->AddButton("Shutdown", Detach);
                sectMain->AddSlider("TestSlider", &float123, 0, 20);
                sectMain->AddSlider("intslider", &testint3, 0, 10);
                //sectMain->AddCombo("TestCombo", std::vector<std::string>{ "Value1", "Value2", "Value3" }, &testint);
            }

            auto sectMain2 = tab2->AddSection("TestSect2", .5f);
            {
                //sectMain2->AddCombo("TestCombo2", std::vector<std::string>{ "ttest", "ttest2", "ttest3" }, &testint2);
                sectMain2->AddCheckBox("Bunnyhop Enabled", &g_Settings.bBhopEnabled);
                sectMain2->AddCheckBox("Show Player Names", &g_Settings.bShowNames);
            }
        } mainWindow->AddChild(tab2);
    }
    this->vecChildren.push_back(mainWindow);

    /* Create our mouse cursor (one instance only) */
    mouseCursor = std::make_unique<MouseCursor>();

    /* Do the first init run through all of the objects */
    for (auto& it : vecChildren)
        it->Initialize();
}
