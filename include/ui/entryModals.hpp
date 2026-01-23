#pragma once

#include "findReplaceEntry.hpp"

#include "config-utils/shared/config-utils.hpp"

#include "bsml/shared/BSML/Components/ModalView.hpp"
#include "bsml/shared/BSML/Components/Settings/DropdownListSetting.hpp"
#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"

#include "UnityEngine/MonoBehaviour.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, FindSettingsModal, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ModalView>, modalView);

    DECLARE_INSTANCE_FIELD(UnityW<BSML::DropdownListSetting>, algorithmDropdown);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ToggleSetting>, accumulateToggle);
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ToggleSetting>, matchCaseToggle);

    DECLARE_INSTANCE_METHOD(void, HandleAlgorithmDropdownOnChange, StringW value);
    DECLARE_INSTANCE_METHOD(void, HandleAccumulateToggleOnChange, bool value);
    DECLARE_INSTANCE_METHOD(void, HandleMatchCaseToggleOnChange, bool value);

    public:
        FindReplaceEntry* entry;

        void show(FindReplaceEntry* entry);
        void hide();

        static FindSettingsModal* create(UnityEngine::Transform* parent);
};

DECLARE_CLASS_CODEGEN(AnyText::UI, ReplaceSettingsModal, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_FIELD(UnityW<BSML::ModalView>, modalView);

    DECLARE_INSTANCE_FIELD(UnityW<BSML::DropdownListSetting>, fontDropdown);

    DECLARE_INSTANCE_METHOD(void, HandleFontDropdownOnChange, StringW value);

    public:
        FindReplaceEntry* entry;

        void show(FindReplaceEntry* entry);
        void hide();

        static ReplaceSettingsModal* create(UnityEngine::Transform* parent);
};