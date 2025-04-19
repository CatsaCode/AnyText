#include "ui.hpp"
#include "main.hpp"

#include "bsml/shared/BSML.hpp"

using namespace UnityEngine;

namespace AnyText {

    void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(!firstActivation) return;

        auto container = BSML::Lite::CreateScrollableSettingsContainer(self->get_transform());
        auto containerTransform = container->get_transform();
        
        BSML::Lite::CreateText(containerTransform, "Hewwo World UwU");
    }

}
