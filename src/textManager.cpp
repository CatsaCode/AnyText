#include "textManager.hpp"
#include "main.hpp"

#include "configs.hpp"

#include <regex>

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

                // TODO Precalculate entryRegexStr when entry.findString is updated
                std::string entryRegexStr = entry.findString;
                if(entry.findAlgorithm != static_cast<int>(FindAlgorithm::Regex)) {
                    static const std::regex escapeRegex ("[\\+\\*\\?\\^\\$\\\\\\.\\[\\]\\{\\}\\(\\)\\|\\/]");
                    entryRegexStr = std::regex_replace(entryRegexStr, escapeRegex, "\\$&");
                }
                if(entry.findAlgorithm == static_cast<int>(FindAlgorithm::ExactMatch))
                    entryRegexStr = '^' + entryRegexStr + "$";
                std::regex entryRegex (entryRegexStr);

                if(!std::regex_match(replacementState.text, entryRegex)) continue;

                // std::regex_constants::format_first_only (argument for regex_match)
                // std::regex::icase (argument for regex constructor)

                // TODO Assuming partial replacement
                replacementState.text = std::regex_replace(replacementState.text, entryRegex, entry.replaceString);
                replacementState.richText = true;

                hasReplacedText = true;
            }
        }
    }

}