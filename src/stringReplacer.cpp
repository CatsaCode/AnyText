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
        // Used for everything else
        // self->set_richText(true);
        // self->set_text("<size=0>Hehe secret text :3</size>AnyText");
    
        // if(self->text) PaperLogger.info("Parsed: {}", self->GetParsedText());
        // if(self->text) self->text = self->text->ToLower();
    
        // int32_t style = static_cast<int32_t>(self->get_fontStyle());
        // int32_t upper = static_cast<int32_t>(TMPro::FontStyles::UpperCase);
        // style &= ~upper;
        // self->set_fontStyle(style);
    
        if(!text->m_text) return;
    
        Transform* menuTransform = text->get_transform();
        for(int i = 0; i < 3; i++) {
            if(menuTransform->get_parent()) menuTransform = menuTransform->get_parent();
            else {menuTransform = nullptr; break;}
        }
        if(menuTransform && menuTransform->get_name() == "AnyTextMenu") return;
    

        std::string textKey = text->m_text->ToLower();
        // To-do Create algorithm to extract the rich text tags. GetParsedText() can not be relied upon because it uses m_textInfo which has not yet been created at this stage of making the text
    
        if(findReplaceStrings.contains(textKey)) text->m_text = AnyText::findReplaceStrings[textKey];
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