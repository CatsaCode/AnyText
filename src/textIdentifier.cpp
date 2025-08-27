#include "textIdentifier.hpp"
#include "main.hpp"

#include "configs.hpp"
#include "textManager.hpp"

#include "TMPro/TMP_Text.hpp"

using namespace UnityEngine;

namespace AnyText {

    MAKE_HOOK_MATCH(ParseInputText_IdentifyText, &TMPro::TMP_Text::ParseInputText, void,
        TMPro::TMP_Text* self
    ) {
        ParseInputText_IdentifyText(self);

        if(!self || !self->get_text() || !self->get_gameObject()) return;

        PaperLogger.info("ParseInputText_IdentifyText has valid TMP_Text: '{}'", self->get_text());
        
        TextManager* textManager = self->GetComponent<TextManager*>();
        if(textManager && textManager->isApplyingState) {PaperLogger.info("TextManager is already replacing text, aborting"); return;}

        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::ExactMatch && !self->get_text()->Equals(entry.findString)) continue;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::PartialMatch && !self->get_text()->Contains(entry.findString)) continue;
                PaperLogger.info("Matched entry with findAlgorithm: {}, findString: '{}'", entry.findAlgorithm, entry.findString);

                if(!textManager) {PaperLogger.info("TextManager does not exist, adding component"); self->get_gameObject()->AddComponent<TextManager*>();}
                else {
                    PaperLogger.info("TextManager already exists, calling OnChangeText");
                    textManager->OnTextChange();
                }

                return;
            }
        }

        if(textManager) Object::Destroy(textManager);
    }

    void installTextIdentifierHooks() {
        INSTALL_HOOK(PaperLogger, ParseInputText_IdentifyText);
    }

}