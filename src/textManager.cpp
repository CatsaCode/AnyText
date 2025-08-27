#include "textManager.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "UnityEngine/Transform.hpp"

using namespace UnityEngine;

DEFINE_TYPE(AnyText, TextManager);

namespace AnyText {

    void TextManager::OnEnable() {
        PaperLogger.info("TextManager::OnEnable");
        text = GetComponent<TMPro::TMP_Text*>();
        if(!text) {
            PaperLogger.error("Couldn't find TMP_Text component");
            Object::Destroy(this);
            return;
        }

        updateOriginalStateWithDifferences();
        generateReplacementState();
        applyState(replacementState);
    }

    void TextManager::OnDisable() {
        PaperLogger.info("TextManager::OnDisable");
        if(!text || !text->get_text()) return;

        updateOriginalStateWithDifferences();
        applyState(originalState);
    }

    void TextManager::OnTextChange() {
        PaperLogger.info("TextManager::OnTextChange");
        if(!text || !text->get_text()) return;

        updateOriginalStateWithDifferences();
        generateReplacementState();
        applyState(replacementState);
    }

    void TextManager::updateOriginalStateWithDifferences() {
        PaperLogger.info("TextManager::updateOriginalStateWithDifferences");
        if(!text || !text->get_text()) return;

        if(text->get_text() != replacementState.text) originalState.text = std::string(text->get_text());
        if(text->get_fontStyle() != replacementState.fontStyle) originalState.fontStyle = text->get_fontStyle();
        if(text->get_richText() != replacementState.richText) originalState.richText = text->get_richText();
    }

    void TextManager::applyState(const TextState& state) {
        PaperLogger.info("TextManager::applyState ({})", &state == &originalState ? "Original" : (&state == &replacementState ? "Replacement" : "Unknown"));
        if(!text || !text->get_text()) return;

        isApplyingState = true;
        text->set_text(state.text);
        text->set_fontStyle(state.fontStyle);
        text->set_richText(state.richText);
        isApplyingState = false;
    }

    void TextManager::generateReplacementState() {
        PaperLogger.info("TextManager::generateReplacementState");
        if(!text || !text->get_text() || !text->get_transform()) return;
        
        Transform* menuTransform = text->get_transform();
        for(int i = 0; i < 4; i++) {
            if(!menuTransform) break;
            if(menuTransform->get_name() == "AnyTextMenu") return;
            menuTransform = menuTransform->get_parent();
        }

        replacementState = originalState;

        bool hasReplacedText = false;
        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                if(hasReplacedText && !entry.accumulate) continue;
                bool tmpFound = replacementState.text.find(entry.findString) != std::string::npos;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::ExactMatch && (!tmpFound || replacementState.text.size() != entry.findString.size())) continue;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::PartialMatch && !tmpFound) continue;

                // Assuming whole replacement
                PaperLogger.info("Replacing '{}' to '{}'", text->get_text(), entry.replaceString);
                replacementState.text = entry.replaceString;
                replacementState.richText = true;

                hasReplacedText = true;
            }
        }
    }

}