#include "ui/utils.hpp"
#include "main.hpp"
#include "assets.hpp"

#include "custom-types/shared/delegate.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "bsml/shared/BSML.hpp"

#include "HMUI/ImageView.hpp"
#include "HMUI/ButtonStaticAnimations.hpp"
#include "HMUI/NoTransitionsButton.hpp"
#include "HMUI/AnimatedSwitchView.hpp"

#include "UnityEngine/TouchScreenKeyboard.hpp"
#include "UnityEngine/TouchScreenKeyboardType.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/Application.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

namespace AnyText::UI {

    Button* createIconButton(Transform* parent, Sprite* sprite, std::function<void()> onClick) {
        Button* button = BSML::Lite::CreateUIButton(parent, "", {0.0, 0.0}, {6, 8}, onClick);
        ImageView* icon = BSML::Lite::CreateImage(button->get_transform(), sprite);
        icon->set_preserveAspect(true);
        icon->get_transform()->set_localScale({0.8, 0.8, 1});
        icon->set_raycastTarget(false);
        icon->set_color(Color(1, 1, 1, 0.75));
        icon->_skew = 0.18;

        // Would be nice to modify the ButtonStaticAnimations AnimationClips directly, but they are baked in some way
        NoTransitionsButton* noTransitionsButton = button->GetComponent<NoTransitionsButton*>();
        noTransitionsButton->add_selectionStateDidChangeEvent(custom_types::MakeDelegate<System::Action_1<NoTransitionsButton::SelectionState>*>(std::function<void(NoTransitionsButton::SelectionState)>(
            [icon](NoTransitionsButton::SelectionState state){
                switch(state) {
                    case NoTransitionsButton::SelectionState::Normal:
                        icon->set_color(Color(1, 1, 1, 0.75));
                        break;
                    case NoTransitionsButton::SelectionState::Disabled:
                        icon->set_color(Color(1, 1, 1, 0.25));
                        break;
                    default:
                        icon->set_color(Color(1, 1, 1, 1));
                }
            }
        )));

        return button;
    }

    InputFieldView* createStringSettingWithSystemKeyboard(Transform* parent, std::string_view placeholder /*= ""*/, std::string_view value /*= ""*/, std::function<void(std::string)> onChange /*= nullptr*/) {
        InputFieldView* stringSetting = BSML::Lite::CreateStringSetting(parent, placeholder, value, onChange);
        GameObject* systemKeyboardButtonGO = GameObject::Instantiate(stringSetting->_clearSearchButton->get_gameObject(), stringSetting->_clearSearchButton->get_transform()->GetParent());
        stringSetting->_clearSearchButton->get_gameObject()->SetActive(false);
        systemKeyboardButtonGO->set_name("SystemKeyboardButton");
        
        Button* systemKeyboardButton = systemKeyboardButtonGO->GetComponent<Button*>();
        systemKeyboardButton->set_interactable(true);
        systemKeyboardButton->get_onClick()->RemoveAllListeners();
        systemKeyboardButton->get_onClick()->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(std::function<void()>([stringSetting](){
            auto coro = custom_types::Helpers::CoroutineHelper::New(openSystemKeyboard(stringSetting));
            stringSetting->StartCoroutine(coro);
        })));

        RectTransform* systemKeyboardButtonBackgroundTransform = systemKeyboardButton->get_transform()->Find("BG")->GetComponent<RectTransform*>();
        systemKeyboardButtonBackgroundTransform->set_sizeDelta({5, 5});
        RectTransform* systemKeyboardButtonIconTransform = systemKeyboardButton->get_transform()->Find("Icon")->GetComponent<RectTransform*>();
        systemKeyboardButtonIconTransform->set_sizeDelta({5, 5});
        ImageView* systemKeyboardButtonIcon = systemKeyboardButtonIconTransform->GetComponent<ImageView*>();
        systemKeyboardButtonIcon->set_sprite(PNG_SPRITE(keyboard));

        return stringSetting;
    }



    custom_types::Helpers::Coroutine openSystemKeyboard(InputFieldView* inputFieldView, TouchScreenKeyboardType type /*= TouchScreenKeyboardType::Default*/, std::string_view placeholder /*= ""*/, int charLimit /*= 0*/) {
        static TouchScreenKeyboard* systemKeyboard = nullptr;
        if(!systemKeyboard) systemKeyboard = TouchScreenKeyboard::Open(inputFieldView->get_text(), type, true, false, false, false, placeholder, charLimit);
        
        float startTimeSeconds = Time::get_realtimeSinceStartup();
        while(
            systemKeyboard && 
            systemKeyboard->status != TouchScreenKeyboard::Status::Done &&
            systemKeyboard->status != TouchScreenKeyboard::Status::Canceled &&
            systemKeyboard->status != TouchScreenKeyboard::Status::LostFocus
        ) {
            co_yield nullptr;
        }
        float endTimeSeconds = Time::get_realtimeSinceStartup();
        
        if(endTimeSeconds - startTimeSeconds < 0.1) openURL("https://github.com/CatsaCode/AnyText/blob/main/docs/repatchInstructions/README.md");

        if(systemKeyboard->status == TouchScreenKeyboard::Status::Done) {
            inputFieldView->set_text(systemKeyboard->get_text());
            inputFieldView->get_onValueChanged()->Invoke(inputFieldView);
        }
        systemKeyboard = nullptr;
        co_return;
    }

    void skipToggleTransition(AnimatedSwitchView* animatedSwitchView) {
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

    // Modified version of MetaCore::UI::AddModalAnimations that adjusts alpha instead of mostly color
    void addModalAnimations(SimpleTextDropdown* dropdown, ModalView* modal) {
        std::function<void(bool)> animateModal = [modal](bool out){
            CanvasGroup* canvasGroup = modal->GetComponent<CanvasGroup*>();
            canvasGroup->set_alpha(out ? 0.6 : 1);
        };

        dropdown->_button->onClick->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(std::function<void()>([animateModal](){
            animateModal(true);
        })));
        dropdown->add_didSelectCellWithIdxEvent(custom_types::MakeDelegate<System::Action_2<UnityW<DropdownWithTableView>, int>*>(std::function<void(UnityW<DropdownWithTableView>, int)>([animateModal](UnityW<DropdownWithTableView>, int){
            animateModal(false);
        })));
        dropdown->_modalView->add_blockerClickedEvent(custom_types::MakeDelegate<System::Action*>(std::function<void()>([animateModal](){
            animateModal(false);
        })));

        if(auto modal = il2cpp_utils::try_cast<HMUI::ModalView>(dropdown->_modalView.unsafePtr()).value_or(nullptr))
            modal->_animateParentCanvas = false;
    }

    void openURL(std::string_view url) {
        #ifdef BS_1_37_0
        static auto OpenURL = il2cpp_utils::resolve_icall<void, StringW>("UnityEngine.Application::OpenURL");
        OpenURL(url);
        #else
        UnityEngine::Application::OpenURL(url);
        #endif
    }

}