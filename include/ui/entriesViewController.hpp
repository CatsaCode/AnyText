#pragma once

#include "configs.hpp"
#include "ui/entryTable.hpp"

#include "config-utils/shared/config-utils.hpp"

#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, EntriesViewController, HMUI::ViewController) {
    DECLARE_INSTANCE_FIELD(UnityW<EntryTableView>, entryTableView);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
};