#include "ui.hpp"
#include "main.hpp"

#include "bsml/shared/BSML.hpp"

#include "UnityEngine/UI/LayoutElement.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;

namespace AnyText {

    GameObject* CreateFindReplaceSetting(Transform* parent) {
        auto row = BSML::Lite::CreateHorizontalLayoutGroup(parent);
        auto rowTransform = row->get_transform();
        auto rowLayoutE = row->GetComponent<LayoutElement*>();
        rowLayoutE->set_preferredWidth(100);
        rowLayoutE->set_preferredHeight(10);

        for(int i = 0; i < 2; i++) {
            auto stringSetting = BSML::Lite::CreateStringSetting(row, "Find", "", nullptr);
            Object::DestroyImmediate(stringSetting->GetComponent<LayoutElement*>());
        }

        return row->get_gameObject();
    }

    void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(!firstActivation) return;

        auto container = BSML::Lite::CreateScrollableSettingsContainer(self->get_transform())->GetComponent<VerticalLayoutGroup*>();
        container->set_name("AnyText Menu");
        container->set_spacing(-3.0);
        auto containerTransform = container->get_transform();
        
        for(int i = 0; i < 15; i++) CreateFindReplaceSetting(containerTransform);
    }

}
