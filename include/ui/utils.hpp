#pragma once

#include "custom-types/shared/coroutine.hpp"

#include "HMUI/InputFieldView.hpp"
#include "HMUI/AnimatedSwitchView.hpp"
#include "HMUI/SimpleTextDropdown.hpp"
#include "HMUI/ModalView.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/TouchScreenKeyboardType.hpp"

namespace AnyText::UI {

    UnityEngine::UI::Button* createIconButton(UnityEngine::Transform* parent, UnityEngine::Sprite* sprite, std::function<void()> onClick);
    HMUI::InputFieldView* createStringSettingWithSystemKeyboard(UnityEngine::Transform* parent, std::string_view placeholder = "", std::string_view value = "", std::function<void(std::string)> onChange = nullptr);

    custom_types::Helpers::Coroutine openSystemKeyboard(HMUI::InputFieldView* inputFieldView, UnityEngine::TouchScreenKeyboardType type = UnityEngine::TouchScreenKeyboardType::Default, std::string_view placeholder = "", int charLimit = 0);
    void skipToggleTransition(HMUI::AnimatedSwitchView* animatedSwitchView);
    void addModalAnimations(HMUI::SimpleTextDropdown* dropdown, HMUI::ModalView* modal);

}