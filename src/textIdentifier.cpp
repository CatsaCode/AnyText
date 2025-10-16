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

        // Transform* menuTransform = text->get_transform();
        // for(int i = 0; i < 4; i++) {
        //     if(!menuTransform) break;
        //     if(menuTransform->get_name() == "AnyTextMenu") return;
        //     menuTransform = menuTransform->get_parent();
        // }
        
        TextManager* textManager = text->GetComponent<TextManager*>();
        std::string identifyingText = textManager ? textManager->getOriginalState().text : std::string(text->get_text());

        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                if(!std::regex_search(identifyingText, entry.getFindRegex())) continue;

                if(!textManager) {
                    PaperLogger.debug("Adding TextManager to '{}'", identifyingText);
                    text->get_gameObject()->AddComponent<TextManager*>();
                }
                else {
                    PaperLogger.debug("Updating TextManager for '{}'", identifyingText);
                    textManager->OnTextChange();
                }

                return;
            }
        }

        PaperLogger.debug("Could not match '{}'", identifyingText);
        if(textManager) {
            PaperLogger.debug("Destroying TextManager");
            Object::Destroy(textManager);
        }
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