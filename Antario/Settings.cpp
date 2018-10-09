#include <functional>
#include <ShlObj.h>

#include "Settings.h"
#include "Hooks.h"


/**
 * \brief Used to initialize settings parser. Call inside Hooks::init AFTER you initialized menu.
 * \param pMenuObj Pointer to your created menu object. In our case, its g_Hooks.nMenu
 */
void Settings::Initialize(MenuMain* pMenuObj)
{
    this->fsPath = this->GetFolderPath();
    /* Create directory in AppData/Roaming if it doesnt exist             */
    create_directories(this->fsPath);       

    /* Setup a proper path to default file.                               */
    auto fsDefaultPath = this->fsPath;
    fsDefaultPath.append("default.xml");

    /* Check if the default config file exists, if it doesnt - create it. */
    if (!exists(fsDefaultPath))
        this->SaveSettings(fsDefaultPath.string(), pMenuObj);

    /* Load default settings. These can be changed by user.                */
    this->LoadSettings(fsDefaultPath.string(), pMenuObj);

    this->UpdateDirectoryContent(this->fsPath);
}


/**
 * \brief Saves settings using menu child system.
 * \param strFileName Name of the file you want your setting to be saved as
 * \param pMenuObj Pointer to your created menu object. In this case, its g_Hooks.nMenu
 */
void Settings::SaveSettings(const std::string& strFileName, MenuMain* pMenuObj)
{
    /* Loop through menu content and get its values */
    std::function<void(MenuMain* /*, Parent pointer for sections in cfgFile*/)> loopChildSettings;
    loopChildSettings = [&loopChildSettings](MenuMain* pMenuObj/*, Parent/section pointer for sections in cfgFile*/) -> void
    {
        /* Replaces spaces with underscores, useful with xml parsing, can be deleted otherwise. */
        auto fixedSpaces = [](std::string str) -> std::string
        {
            auto tmpString = str;
            std::replace(tmpString.begin(), tmpString.end(), ' ', '_');
            return tmpString;
        };

        for (auto& it : pMenuObj->vecChildren)
        {
            switch (it->type)
            {
                    /*
                    * Ready for future sub-sections or tab settings.
                    * Just create a new `MenuSelectableType` for it
                    * and loop like in TYPE_SECTION to create sub-settings.
                    */
            case MST::TYPE_INCORR:
            case MST::TYPE_WINDOW:
                {
                    /* Recurrent call so we save settings for all of the child objects. */
                    loopChildSettings(it.get()/*, parentElement*/);
                    break;
                }
            case MST::TYPE_SECTION:
                {
                    /*
                    * Create section inside your file to which you will assign child objects (child elements, w/e)
                    * Then loop through child objects and save them. You can use it->strLabel to get section name.
                    */
                    loopChildSettings(it.get()/*, SectionElement*/);
                    break;
                }
            case MST::TYPE_CHECKBOX:
                {
                    auto tmpChkbx = dynamic_cast<Checkbox*>(it.get());
                    /*
                    * Save checkbox value into your settings. Setting parent should be transfered in lambda call
                    * You can use tmpChkbx->strLabel to get the name and tmpChkbx->bCheckboxValue to get true/false.
                    */
                    break;
                }
            case MST::TYPE_COMBO:
                {
                    //auto tmpCombo = dynamic_cast<ComboBox*>(it.get());
                    /*
                    * Save combo index into your settings. Setting parent should be transfered in lambda call
                    * You can use tmpCombo->strLabel to get the name and tmpCombo->iCurrentValue to get the index.
                    */
                    break;
                }
            default: break;
            }
        }
    };
    loopChildSettings(pMenuObj/*, rootElement*/); /* Cant inline, drops an error */
}

void Settings::LoadSettings(const std::string& strFileName, MenuMain* pMenuObj)
{
    /*
     * Use the code from "SaveSettings" but instead of saving, load them
     * Not hard huh?
     */
}


/**
 * \brief Updates vector with file names so you can use custom configs made by users (unlimited)
 * \param fsPath Path to the folder containing user configs
 */
void Settings::UpdateDirectoryContent(const fs::path& fsPath)
{
    this->vecFileNames.clear();

    /* Loop through directory content and save config files to your vector */
    for (const auto& it : fs::directory_iterator(fsPath))
    {
        if (!is_empty(it) && fsPath.extension() == ".yourextension")
            this->vecFileNames.push_back(it.path().filename().string());
    }
}


/**
 * \brief Gets the path to the userconfig folder. In this case - %APPDATA%\Antario\configs
 * \return std::experimental::filesystem::path containing folder path.
 */
fs::path Settings::GetFolderPath()
{
    /* Get %APPDATA% path in Windows system. */
    TCHAR szPath[MAX_PATH];
    SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, szPath);

    /* Save as filesystem::path and extend it so we use our own folder.*/
    auto fsPath = fs::path(szPath);
    fsPath.append(R"(\Antario\configs\)");
    return fsPath;
}
