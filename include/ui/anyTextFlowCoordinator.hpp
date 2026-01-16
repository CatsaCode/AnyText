#pragma once

#include "configs.hpp"

#include "custom-types/shared/macros.hpp"

#include "ui/configListViewController.hpp"
#include "ui/entryListViewController.hpp"

#include "HMUI/FlowCoordinator.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, AnyTextFlowCoordinator, HMUI::FlowCoordinator) {
    DECLARE_INSTANCE_FIELD(UnityW<AnyText::UI::ConfigListViewController>, configListViewController);
    DECLARE_INSTANCE_FIELD(UnityW<AnyText::UI::EntryListViewController>, entryListViewController);

    DECLARE_INSTANCE_FIELD_DEFAULT(bool, presentingEntries, false);

    DECLARE_INSTANCE_METHOD(void, Awake);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::FlowCoordinator::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_OVERRIDE_METHOD_MATCH(void, BackButtonWasPressed, &HMUI::FlowCoordinator::BackButtonWasPressed, HMUI::ViewController* topViewController);

    DECLARE_STATIC_METHOD(void, Present);

    public:
        static UnityW<AnyTextFlowCoordinator> instance;

        void presentEntries(Config* config);
        void presentConfigs();
};