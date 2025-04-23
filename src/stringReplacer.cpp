#include "stringReplacer.hpp"
#include "main.hpp"

#include "UnityEngine/Transform.hpp"

#include "TMPro/TMP_Text.hpp"
#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include <string>

using namespace UnityEngine;

namespace AnyText {

    std::map<std::string, std::string> findReplaceStrings = {
        // {"play", "Yeet!"},
        // {"practice", "Cheat"},
        // {"solo", "Alone"}
    };

    void RevertText(TMPro::TMP_Text* text) {
        if(!text->m_text) return;

        if(!text->m_text->StartsWith("<size=0>AnyText_")) return;


        int underscore1 = 15;
        int underscore2 = text->m_text->IndexOf('_', underscore1 + 1);
        int underscore3 = text->m_text->IndexOf('_', underscore2 + 1);
        
        std::string originalStyleString = text->m_text->Substring(underscore1 + 1, underscore2 - underscore1 - 1);
        int32_t originalStyle = std::stoi(originalStyleString);
        std::string originalText = text->m_text->Substring(underscore2 + 1, underscore3 - underscore2 - 1);


        text->m_fontStyle = originalStyle;
        text->m_text = originalText;
    }

    void ReplaceText(TMPro::TMP_Text* text) {
        if(!text->m_text) return;
    
        Transform* menuTransform = text->get_transform();
        for(int i = 0; i < 3; i++) {
            if(menuTransform->get_parent()) menuTransform = menuTransform->get_parent();
            else {menuTransform = nullptr; break;}
        }
        if(menuTransform && menuTransform->get_name() == "AnyTextMenu") return;

        std::string textKey = text->m_text->ToLower();
        // To-do Create algorithm to extract the rich text tags. GetParsedText() can not be relied upon because it uses m_textInfo which has not yet been created at this stage of making the text
        if(!findReplaceStrings.contains(textKey)) return;
        

        // Private variables are set directly because the public setters will call SetVerticesDirty(), creating an infinite loop
        text->m_isRichText = true;

        // Remove the uppercase bit from the fontStyle
        // To-do if the text "practice" is replaced, all of the buttons that use the practice button as a template will have this bit removed and it won't get put back.. ex: the Mods section in the main menu
        int32_t oldStyle = static_cast<int32_t>(text->get_fontStyle());
        int32_t newStyle = oldStyle;
        int32_t upper = static_cast<int32_t>(TMPro::FontStyles::UpperCase);
        newStyle &= ~upper;
        text->m_fontStyle = newStyle;

        text->m_text = fmt::format("<size=0>AnyText_{}_{}_AnyText</size>{}", oldStyle, text->m_text, findReplaceStrings[textKey]);
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
        INSTALL_HOOK(PaperLogger, TextMeshProHook);
        INSTALL_HOOK(PaperLogger, TextMeshProUGUIHook);
    }

}