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

        Transform* menuTransform = self->get_transform();
        for(int i = 0; i < 4; i++) {
            if(!menuTransform) break;
            if(menuTransform->get_name() == "AnyTextMenu") return;
            menuTransform = menuTransform->get_parent();
        }

        PaperLogger.info("ParseInputText_IdentifyText has valid TMP_Text: '{}'", self->get_text());
        
        TextManager* textManager = self->GetComponent<TextManager*>();
        if(textManager && textManager->isApplyingState) {PaperLogger.info("TextManager is already replacing text, aborting"); return;}

        std::string originalText = textManager ? textManager->originalState.text : std::string(self->get_text());

        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                bool tmpFound = originalText.find(entry.findString) != std::string::npos;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::ExactMatch && (!tmpFound || originalText.size() != entry.findString.size())) continue;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::PartialMatch && !tmpFound) continue;
                PaperLogger.info("Original text '{}' matched entry with findAlgorithm: {}, findString: '{}'", originalText, entry.findAlgorithm, entry.findString);

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