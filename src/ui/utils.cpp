#include "ui/utils.hpp"
#include "main.hpp"

#include "custom-types/shared/delegate.hpp"

#include "bsml/shared/BSML.hpp"

#include "HMUI/ImageView.hpp"
#include "HMUI/ButtonStaticAnimations.hpp"
#include "HMUI/NoTransitionsButton.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

namespace AnyText::UI {

    Button* createIconButton(Transform* parent, Sprite* sprite, std::function<void()> onClick) {
        Button* button = BSML::Lite::CreateUIButton(parent, "", {0.0, 0.0}, {6, 8}, onClick);
        ImageView* icon = BSML::Lite::CreateImage(button->get_transform(), sprite);
        icon->set_preserveAspect(true);
        icon->get_transform()->set_localScale({0.8, 0.8, 1});
        icon->set_color(Color(1, 1, 1, 0.75));
        icon->_skew = 0.18;

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

}