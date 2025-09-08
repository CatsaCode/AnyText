#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/RectTransform.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, ConfigsViewController, HMUI::ViewController) {
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::RectTransform>, configsVerticalTransform);

    DECLARE_OVERRIDE_METHOD_MATCH(void, DidActivate, &HMUI::ViewController::DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
};