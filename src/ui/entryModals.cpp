#include "ui/entryModals.hpp"
#include "main.hpp"

#include "findReplaceEntry.hpp"
#include "fontLoader.hpp"
#include "ui/utils.hpp"

#include "bsml/shared/BSML/Components/ExternalComponents.hpp"

#include "HMUI/ModalView.hpp"
#include "HMUI/SimpleTextDropdown.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/Toggle.hpp"

#include <functional>

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, FindSettingsModal);
DEFINE_TYPE(AnyText::UI, ReplaceSettingsModal);

namespace AnyText::UI {

    static constexpr std::string baseFontLabel = "Default (Teko)";
    static constexpr std::string retainLabel = "Retain"; // TODO Retain / keep / same, find a better name

    FindSettingsModal* FindSettingsModal::create(Transform* parent) {
        PaperLogger.debug("FindSettingsModal");

        BSML::ModalView* modalView = BSML::Lite::CreateModal(parent, {0, 0}, {70, 40}, nullptr);
        RectTransform* containerTransform = BSML::Lite::CreateScrollableModalContainer(modalView)->get_transform().cast<RectTransform>();
        containerTransform->GetComponent<BSML::ExternalComponents*>()->Get<RectTransform*>()->set_anchoredPosition({4, 0}); // The stupid centering™

        GameObject* findSettingsModalGO = modalView->get_gameObject();
        RectTransform* findSettingsModalTransform = findSettingsModalGO->get_transform().cast<RectTransform>();
        FindSettingsModal* findSettingsModal = findSettingsModalGO->AddComponent<FindSettingsModal*>();
        findSettingsModal->modalView = modalView;

        findSettingsModal->algorithmDropdown = BSML::Lite::CreateDropdown(containerTransform, "Algorithm", "", FindAlgorithm_Strings, std::bind(&FindSettingsModal::HandleAlgorithmDropdownOnChange, findSettingsModal, std::placeholders::_1));
        findSettingsModal->accumulateToggle = BSML::Lite::CreateToggle(containerTransform, "Accumulate", false, std::bind(&FindSettingsModal::HandleAccumulateToggleOnChange, findSettingsModal, std::placeholders::_1));
        findSettingsModal->matchCaseToggle = BSML::Lite::CreateToggle(containerTransform, "Match Case", false, std::bind(&FindSettingsModal::HandleMatchCaseToggleOnChange, findSettingsModal, std::placeholders::_1));

        return findSettingsModal;
    }

    void FindSettingsModal::HandleAlgorithmDropdownOnChange(StringW value) {
        PaperLogger.debug("&FindSettingsModal: {}", static_cast<void*>(this));
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        int index = std::distance(FindAlgorithm_Strings.begin(), std::find(FindAlgorithm_Strings.begin(), FindAlgorithm_Strings.end(), value));
        if(index == FindAlgorithm_Strings.size()) {PaperLogger.error("value not found in array"); return;}
        entry->setFindAlgorithm(static_cast<FindAlgorithm>(index));
    }

    void FindSettingsModal::HandleAccumulateToggleOnChange(bool value) {
        PaperLogger.debug("&FindSettingsModal: {}", static_cast<void*>(this));
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        entry->accumulate = value;
    }

    void FindSettingsModal::HandleMatchCaseToggleOnChange(bool value) {
        PaperLogger.debug("&FindSettingsModal: {}", static_cast<void*>(this));
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        entry->setMatchCase(value);
    }
    
    void FindSettingsModal::show(FindReplaceEntry* entry) {
        PaperLogger.debug("&FindSettingsModal: {}, &entry: {}", static_cast<void*>(this), static_cast<void*>(entry));
        if(!entry) {PaperLogger.error("entry is nullptr"); return;}
        this->entry = entry;

        if(!modalView) {PaperLogger.error("modalView is not assigned"); return;}
        
        int findAlgorithmIndex = static_cast<int>(entry->getFindAlgorithm());
        if(findAlgorithmIndex < 0 || findAlgorithmIndex >= algorithmDropdown->values.size()) {PaperLogger.warn("findAlgorithmIndex {} is invalid for size {}, resetting to 0...", findAlgorithmIndex, algorithmDropdown->values.size()); 
            findAlgorithmIndex = 0;}
        algorithmDropdown->set_Value(algorithmDropdown->values[findAlgorithmIndex]);
        
        accumulateToggle->set_Value(entry->accumulate);
        skipToggleTransition(accumulateToggle->GetComponentInChildren<AnimatedSwitchView*>());

        matchCaseToggle->set_Value(entry->getMatchCase());
        skipToggleTransition(matchCaseToggle->GetComponentInChildren<AnimatedSwitchView*>());
        
        modalView->Show();
    }

    void FindSettingsModal::hide() {
        PaperLogger.debug("&FindSettingsModal: {}", static_cast<void*>(this));
        if(!modalView) {PaperLogger.error("modalView is not assigned"); return;}
        modalView->Hide();
    }



    std::vector<std::string_view>& getFontLabels() {
        static std::vector<std::string_view> fontLabels = {};
        if(fontLabels.empty()) {
            fontLabels = {retainLabel, baseFontLabel};
            fontLabels.reserve(fontAssets.size() + 1);
            for(auto& fontAsset : fontAssets) if(fontAsset.first != baseFontName) fontLabels.push_back(fontAsset.first);
        }
        return fontLabels;
    }

    ReplaceSettingsModal* ReplaceSettingsModal::create(Transform* parent) {
        PaperLogger.debug("ReplaceSettingsModal");

        BSML::ModalView* modalView = BSML::Lite::CreateModal(parent, {0, 0}, {70, 40}, nullptr);
        RectTransform* containerTransform = BSML::Lite::CreateScrollableModalContainer(modalView)->get_transform().cast<RectTransform>();
        containerTransform->GetComponent<BSML::ExternalComponents*>()->Get<RectTransform*>()->set_anchoredPosition({4, 0}); // The stupid centering™

        GameObject* replaceSettingsModalGO = modalView->get_gameObject();
        RectTransform* replaceSettingsModalTransform = replaceSettingsModalGO->get_transform().cast<RectTransform>();
        ReplaceSettingsModal* replaceSettingsModal = replaceSettingsModalGO->AddComponent<ReplaceSettingsModal*>();
        replaceSettingsModal->modalView = modalView;

        replaceSettingsModal->fontDropdown = BSML::Lite::CreateDropdown(containerTransform, "Font", "", getFontLabels(), std::bind(&ReplaceSettingsModal::HandleFontDropdownOnChange, replaceSettingsModal, std::placeholders::_1));

        return replaceSettingsModal;
    }

    void ReplaceSettingsModal::HandleFontDropdownOnChange(StringW value) {
        PaperLogger.debug("&ReplaceSettingsModal: {}", static_cast<void*>(this));
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}

        fontDropdown->dropdown->_text->set_color({1, 1, 1, 1});
        if(value == retainLabel) {
            entry->fontName = std::nullopt;
            return;
        }
        if(value == baseFontLabel) value = baseFontName;
        if(!fontAssets.contains(value)) {PaperLogger.error("fontAssets does not contain '{}'", value); return;}
        entry->fontName = value;
    }

    void ReplaceSettingsModal::show(FindReplaceEntry* entry) {
        PaperLogger.debug("&ReplaceSettingsModal: {}, &entry: {}", static_cast<void*>(this), static_cast<void*>(entry));
        if(!entry) {PaperLogger.error("entry is nullptr"); return;}
        this->entry = entry;

        if(!modalView) {PaperLogger.error("modalView is not assigned"); return;}

        std::vector<std::string_view>& fontLabels = getFontLabels();
        std::string fontLabel = !entry->fontName ? retainLabel : (entry->fontName == baseFontName ? baseFontLabel : entry->fontName.value());
        int fontLabelIndex = std::distance(fontLabels.begin(), std::find(fontLabels.begin(), fontLabels.end(), fontLabel));
        if(fontLabelIndex < fontLabels.size()) {
            fontDropdown->set_Value(fontDropdown->values[fontLabelIndex]);
            fontDropdown->dropdown->_text->set_color({1, 1, 1, 1});
        } else {
            fontDropdown->dropdown->_text->set_text(fontLabel);
            fontDropdown->dropdown->_text->set_color({1, 0.2, 0.2, 1});
        }

        modalView->Show();
    }

    void ReplaceSettingsModal::hide() {
        PaperLogger.debug("&ReplaceSettingsModal: {}", static_cast<void*>(this));
        if(!modalView) {PaperLogger.error("modalView is not assigned"); return;}
        modalView->Hide();
    }

}