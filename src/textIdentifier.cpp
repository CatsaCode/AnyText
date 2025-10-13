#include "textIdentifier.hpp"
#include "main.hpp"

#include "configs.hpp"
#include "textManager.hpp"

#include "TMPro/TMP_Text.hpp"

#include <regex>

using namespace UnityEngine;

namespace AnyText {

    void identifyText(TMPro::TMP_Text* text) {
        if(!text || !text->get_text() || !text->get_gameObject()) return;

        Transform* menuTransform = text->get_transform();
        for(int i = 0; i < 4; i++) {
            if(!menuTransform) break;
            if(menuTransform->get_name() == "AnyTextMenu") return;
            menuTransform = menuTransform->get_parent();
        }
        
        TextManager* textManager = text->GetComponent<TextManager*>();

        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                // TODO Precalculate entryRegexStr when entry.findString is updated
                std::string entryRegexStr = entry.findString;
                if(entry.findAlgorithm != static_cast<int>(FindAlgorithm::Regex)) {
                    static const std::regex escapeRegex ("[\\+\\*\\?\\^\\$\\\\\\.\\[\\]\\{\\}\\(\\)\\|\\/]");
                    entryRegexStr = std::regex_replace(entryRegexStr, escapeRegex, "\\$&");
                }
                if(entry.findAlgorithm == static_cast<int>(FindAlgorithm::ExactMatch))
                    entryRegexStr = '^' + entryRegexStr + "$";
                std::regex entryRegex (entryRegexStr);
                if(!std::regex_match(std::string(text->get_text()), entryRegex)) continue;

                if(!textManager) text->get_gameObject()->AddComponent<TextManager*>();
                else textManager->OnTextChange();

                return;
            }
        }

        if(textManager) Object::Destroy(textManager);
    }

    MAKE_HOOK_MATCH(ParseInputText_IdentifyText, &TMPro::TMP_Text::ParseInputText, void,
        TMPro::TMP_Text* self
    ) {
        identifyText(self);
        ParseInputText_IdentifyText(self);
    }

    void installTextIdentifierHooks() {
        INSTALL_HOOK(PaperLogger, ParseInputText_IdentifyText);
    }

}