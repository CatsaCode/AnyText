#include "textManager.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "UnityEngine/Transform.hpp"

using namespace UnityEngine;

DEFINE_TYPE(AnyText, TextManager);

namespace AnyText {

    void TextManager::OnEnable() {
        PaperLogger.info("TextManager::OnEnable");
    }

    void TextManager::OnDisale() {
        PaperLogger.info("TextManager::OnDisable");
    }

    void TextManager::SaveText() {
        PaperLogger.info("TextManager::SaveText");
        if(!text) return;
        prevTextStr = text->get_text();
        prevFontStyle = text->get_fontStyle();
        prevRichText = text->get_richText();
    }

    void TextManager::RestoreText() {
        PaperLogger.info("TextManager::RestoreText");
        if(!text) return;
        text->set_text(prevTextStr);
        text->set_fontStyle(prevFontStyle);
        text->set_richText(prevRichText);
    }

    void TextManager::ReplaceText() {
        PaperLogger.info("TextManager::ReplaceText");
        if(!text) return;
        
        Transform* menuTransform = text->get_transform();
        for(int i = 0; i < 4; i++) {
            if(!menuTransform) break;
            if(menuTransform->get_name() == "AnyTextMenu") return;
            menuTransform = menuTransform->get_parent();
        }

        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                
            }
        }
    }

}