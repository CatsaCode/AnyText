#pragma once

#include "custom-types/shared/macros.hpp"

#include "ui/configsViewController.hpp"

#include "HMUI/FlowCoordinator.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, AnyTextFlowCoordinator, HMUI::FlowCoordinator) {
    DECLARE_INSTANCE_FIELD(UnityW<AnyText::UI::ConfigsViewController>, configsViewController);

    DECLARE_INSTANCE_METHOD(void, Awake);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::FlowCoordinator::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed, &HMUI::FlowCoordinator::BackButtonWasPressed, HMUI::ViewController* topViewController);
};