#include "ui/entryModals.hpp"
#include "main.hpp"

#include "findReplaceEntry.hpp"

#include "bsml/shared/BSML/Components/ExternalComponents.hpp"

#include "HMUI/ModalView.hpp"
#include "HMUI/AnimatedSwitchView.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/Toggle.hpp"

#include <functional>

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, FindSettingsModal);

namespace AnyText::UI {

    static void skipToggleTransition(AnimatedSwitchView* animatedSwitchView) {
        if(!animatedSwitchView || !animatedSwitchView->_toggle) {PaperLogger.warn("Failed to skip transition, couldn't find components"); return;}
        animatedSwitchView->_toggle->PlayEffect(true);
        bool isOn = animatedSwitchView->_toggle->get_isOn();
        animatedSwitchView->_switchAmount = isOn;
        animatedSwitchView->_highlightAmount = isOn;
        animatedSwitchView->_disabledAmount = !isOn;
        animatedSwitchView->_animationState = AnimatedSwitchView::AnimationState::Idle;
        animatedSwitchView->LerpPosition(isOn);
        animatedSwitchView->LerpColors(isOn, isOn, !isOn);
        animatedSwitchView->LerpStretch(isOn);
    }


    
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
        
        modalView->Show();
    }

    void FindSettingsModal::hide() {
        PaperLogger.debug("&FindSettingsModal: {}", static_cast<void*>(this));
        if(!modalView) {PaperLogger.error("modalView is not assigned"); return;}
        modalView->Hide();
    }

}