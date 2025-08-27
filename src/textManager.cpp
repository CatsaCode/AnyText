#include "textManager.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "UnityEngine/Transform.hpp"

using namespace UnityEngine;

DEFINE_TYPE(AnyText, TextManager);

namespace AnyText {

    void TextManager::OnEnable() {
        PaperLogger.info("TextManager::OnEnable");
        SaveText();
        ReplaceText();
    }

    void TextManager::OnDisale() {
        PaperLogger.info("TextManager::OnDisable");
        RestoreText();
    }

    void TextManager::SaveText() {
        PaperLogger.info("TextManager::SaveText");
        if(!text || !text->get_text()) return;
        prevTextStr = text->get_text();
        prevFontStyle = text->get_fontStyle();
        prevRichText = text->get_richText();
    }

    void TextManager::RestoreText() {
        PaperLogger.info("TextManager::RestoreText");
        if(!text || !text->get_text()) return;
        text->set_text(prevTextStr);
        text->set_fontStyle(prevFontStyle);
        text->set_richText(prevRichText);
    }

    void TextManager::ReplaceText() {
        PaperLogger.info("TextManager::ReplaceText");
        if(!text || !text->get_text()) return;
        
        Transform* menuTransform = text->get_transform();
        for(int i = 0; i < 4; i++) {
            if(!menuTransform) break;
            if(menuTransform->get_name() == "AnyTextMenu") return;
            menuTransform = menuTransform->get_parent();
        }

        isReplacingText = true;
        bool hasReplacedText = false;
        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                if(hasReplacedText && !entry.accumulate) continue;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::ExactMatch && !text->get_text()->Equals(entry.findString)) continue;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::PartialMatch && !text->get_text()->Contains(entry.findString)) continue;

                // Assuming whole replacement
                text->set_text(entry.replaceString);
                text->set_richText(true);

                hasReplacedText = true;
            }
        }
        isReplacingText = false;
    }

}