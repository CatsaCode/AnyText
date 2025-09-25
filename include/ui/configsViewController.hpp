#pragma once

#include "configTable.hpp"

#include "custom-types/shared/macros.hpp"

#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, ConfigsViewController, HMUI::ViewController) {
    DECLARE_INSTANCE_FIELD(UnityW<AnyText::UI::ConfigTableView>, configTableView);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
};