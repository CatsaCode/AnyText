#include "textManager.hpp"
#include "main.hpp"

#include "configs.hpp"

using namespace UnityEngine;

DEFINE_TYPE(AnyText, TextManager);

namespace AnyText {

    void TextManager::OnEnable() {
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
        if(!text || !text->get_text()) return;

        updateOriginalStateWithDifferences();
        applyState(originalState);
    }

    void TextManager::OnTextChange() {
        if(!text || !text->get_text()) return;

        if(updateOriginalStateWithDifferences()) {
            generateReplacementState();
            applyState(replacementState);
        }
    }

    bool TextManager::updateOriginalStateWithDifferences() {
        if(!text || !text->get_text()) return false;

        bool didChange = false;
        if(text->get_text() != replacementState.text) { originalState.text = std::string(text->get_text()); didChange = true; }
        if(text->get_fontStyle() != replacementState.fontStyle) { originalState.fontStyle = text->get_fontStyle(); didChange = true; }
        if(text->get_richText() != replacementState.richText) { originalState.richText = text->get_richText(); didChange = true; }
        return didChange;
    }

    void TextManager::applyState(const TextState& state) {
        if(!text || !text->get_text()) return;

        text->set_text(state.text);
        text->set_fontStyle(state.fontStyle);
        text->set_richText(state.richText);
    }

    void TextManager::generateReplacementState() {
        if(!text || !text->get_text() || !text->get_transform()) return;

        replacementState = originalState;

        bool hasReplacedText = false;
        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                if(hasReplacedText && !entry.accumulate) continue;
                bool tmpFound = replacementState.text.find(entry.findString) != std::string::npos;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::PartialMatch && !tmpFound) continue;
                if(static_cast<FindAlgorithm>(entry.findAlgorithm) == FindAlgorithm::ExactMatch && (!tmpFound || replacementState.text.size() != entry.findString.size())) continue;

                // Assuming whole replacement
                replacementState.text = entry.replaceString;
                replacementState.richText = true;

                hasReplacedText = true;
            }
        }
    }

}