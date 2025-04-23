#include "stringReplacer.hpp"
#include "main.hpp"

#include "UnityEngine/Transform.hpp"
#include "TMPro/TMP_Text.hpp"
#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace UnityEngine;

namespace AnyText {

    std::map<std::string, std::string> findReplaceStrings = {
        {"play", "Yeet!"},
        {"practice", "Cheat"},
        {"solo", "Alone"}
    };

    void ReplaceText(TMPro::TMP_Text* text) {
        if(!text->m_text) return;
        // if(text->m_text->get_Length() == 0) return;
    
        Transform* menuTransform = text->get_transform();
        for(int i = 0; i < 3; i++) {
            if(menuTransform->get_parent()) menuTransform = menuTransform->get_parent();
            else {menuTransform = nullptr; break;}
        }
        if(menuTransform && menuTransform->get_name() == "AnyTextMenu") return;

        std::string textKey = text->m_text->ToLower();
        // To-do Create algorithm to extract the rich text tags. GetParsedText() can not be relied upon because it uses m_textInfo which has not yet been created at this stage of making the text
        if(!findReplaceStrings.contains(textKey)) return;
        

        text->set_richText(true);

        // To-do save the uppercase bit in here as well
        text->m_text = "<size=0>" + text->m_text + "</size>" + AnyText::findReplaceStrings[textKey]; // Setting m_text directly because text has a potentially recursive setter

        // Remove the uppercase bit from the fontStyle
        // To-do if the text "practice" is replaced, all of the buttons that use the practice button as a template will have this bit removed and it won't get put back.. ex: the Mods section in the main menu
        int32_t style = static_cast<int32_t>(text->get_fontStyle());
        int32_t upper = static_cast<int32_t>(TMPro::FontStyles::UpperCase);
        style &= ~upper;
        text->set_fontStyle(style);
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