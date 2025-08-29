#include "textIdentifier.hpp"
#include "main.hpp"

#include "configs.hpp"
#include "textManager.hpp"

#include "TMPro/TMP_Text.hpp"

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

        PaperLogger.info("identifyText has valid TMP_Text: '{}'", text->get_text());
        
        TextManager* textManager = text->GetComponent<TextManager*>();
        if(textManager && textManager->isApplyingState) {PaperLogger.info("TextManager is already replacing text, aborting"); return;}

        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::ExactMatch && !text->get_text()->Equals(entry.findString)) continue;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::PartialMatch && !text->get_text()->Contains(entry.findString)) continue;
                PaperLogger.info("Matched entry with findAlgorithm: {}, findString: '{}'", entry.findAlgorithm, entry.findString);

                if(!textManager) {PaperLogger.info("TextManager does not exist, adding component"); text->get_gameObject()->AddComponent<TextManager*>();}
                else {
                    PaperLogger.info("TextManager already exists, calling OnChangeText");
                    textManager->OnTextChange();
                }

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