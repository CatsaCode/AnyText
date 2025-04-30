#include "stringReplacer.hpp"
#include "main.hpp"
#include "modConfig.hpp"

#include "floatingLog.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Resources.hpp"

#include "TMPro/TMP_Text.hpp"
#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include <string>

using namespace GlobalNamespace;
using namespace UnityEngine;

namespace AnyText {

    std::map<std::string, std::string> findReplaceStrings = {};

    bool RevertText(TMPro::TMP_Text* text) {
        if(!text->m_text) return false;

        if(!text->m_text->StartsWith("<size=0>AnyText_")) return false;


        int underscore1 = 15;
        int underscore2 = text->m_text->IndexOf('_', underscore1 + 1);
        int underscore3 = text->m_text->IndexOf('_', underscore2 + 1);
        
        std::string originalStyleString = text->m_text->Substring(underscore1 + 1, underscore2 - underscore1 - 1);
        int32_t originalStyle = std::stoi(originalStyleString);
        std::string originalText = text->m_text->Substring(underscore2 + 1, underscore3 - underscore2 - 1);


        text->m_fontStyle = originalStyle;
        text->m_text = originalText;

        return true;
    }

    bool ReplaceText(TMPro::TMP_Text* text) {
        if(!text->m_text) return false;
    
        Transform* menuTransform = text->get_transform();
        for(int i = 0; i < 3; i++) {
            if(menuTransform->get_parent()) menuTransform = menuTransform->get_parent();
            else {menuTransform = nullptr; break;}
        }
        if(menuTransform && menuTransform->get_name() == "AnyTextMenu") return false;

        FloatingLog::AddLine(text->m_text);

        std::string textKey = text->m_text->ToLower();
        // To-do Create algorithm to extract the rich text tags. GetParsedText() can not be relied upon because it uses m_textInfo which has not yet been created at this stage of making the text
        if(!findReplaceStrings.contains(textKey)) return false;
        

        // Private variables are set directly because the public setters will call SetVerticesDirty(), creating an infinite loop
        text->m_isRichText = true;

        // Remove the uppercase bit from the fontStyle
        int32_t oldStyle = static_cast<int32_t>(text->get_fontStyle());
        int32_t newStyle = oldStyle;
        int32_t upper = static_cast<int32_t>(TMPro::FontStyles::UpperCase);
        newStyle &= ~upper;
        text->m_fontStyle = newStyle;

        text->m_text = fmt::format("<size=0>AnyText_{}_{}_AnyText</size>{}", oldStyle, text->m_text, findReplaceStrings[textKey]);

        return true;
    }



    MAKE_HOOK_MATCH(InitialReplaceHook, &MainMenuViewController::DidActivate, void,
    MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        InitialReplaceHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);

        if(!firstActivation) return;

        std::vector<std::string> findStrings = getModConfig().findStrings.GetValue();
        std::vector<std::string> replaceStrings = getModConfig().replaceStrings.GetValue();
        for(int i = 0; i < std::min(findStrings.size(), replaceStrings.size()); i++) {
            StringW key = findStrings[i];
            key = key->ToLower();
            findReplaceStrings[key] = replaceStrings[i];
        }

        for(TMPro::TMP_Text* text : Resources::FindObjectsOfTypeAll<TMPro::TMP_Text*>()) {
            if(ReplaceText(text)) text->SetAllDirty();
        }
    }

    MAKE_HOOK_MATCH(TextMeshProHook, &TMPro::TextMeshPro::SetVerticesDirty, void, 
    TMPro::TextMeshPro* self) {
        ReplaceText(self);
        TextMeshProHook(self);
    }
    
    MAKE_HOOK_MATCH(TextMeshProUGUIHook, &TMPro::TextMeshProUGUI::SetVerticesDirty, void, 
    TMPro::TextMeshProUGUI* self) {
        ReplaceText(self);
        TextMeshProUGUIHook(self);
    }

    void InstallStringReplacerHooks() {
        INSTALL_HOOK(PaperLogger, InitialReplaceHook);
        INSTALL_HOOK(PaperLogger, TextMeshProHook);
        INSTALL_HOOK(PaperLogger, TextMeshProUGUIHook);
    }

}