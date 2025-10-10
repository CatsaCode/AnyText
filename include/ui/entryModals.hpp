#pragma once

#include "configs.hpp"

#include "config-utils/shared/config-utils.hpp"

#include "bsml/shared/BSML/Components/ModalView.hpp"
#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"

#include "UnityEngine/MonoBehaviour.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, FindSettingsModal, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ModalView>, modalView);

    DECLARE_INSTANCE_FIELD(UnityW<BSML::ToggleSetting>, accumulateToggle);

    DECLARE_INSTANCE_METHOD(void, HandleAccumulateToggleOnToggle, bool value);

    public:
        FindReplaceEntry* entry;

        void show(FindReplaceEntry* entry);
        void hide();

        static FindSettingsModal* create(UnityEngine::Transform* parent);
};