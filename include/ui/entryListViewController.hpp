#pragma once

#include "configs.hpp"
#include "ui/entryTable.hpp"
#include "ui/entryModals.hpp"

#include "config-utils/shared/config-utils.hpp"

#include "HMUI/ViewController.hpp"
#include "HMUI/ModalView.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, EntryListViewController, HMUI::ViewController) {
    DECLARE_INSTANCE_FIELD(UnityW<EntryTableView>, entryTableView);
    DECLARE_INSTANCE_FIELD(UnityW<FindSettingsModal>, findSettingsModal);
    DECLARE_INSTANCE_FIELD(UnityW<ReplaceSettingsModal>, replaceSettingsModal);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);

    public:
        void setConfig(Config* config);
};