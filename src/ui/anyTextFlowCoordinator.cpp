#include "ui/anyTextFlowCoordinator.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "ui/configsViewController.hpp"

#include "bsml/shared/Helpers/creation.hpp"

DEFINE_TYPE(AnyText::UI, AnyTextFlowCoordinator);

namespace AnyText::UI {

    void AnyTextFlowCoordinator::Awake() {
        if(!configsViewController) configsViewController = BSML::Helpers::CreateViewController<ConfigsViewController*>();
    }

    void AnyTextFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(!firstActivation) return;

        SetTitle("AnyText", HMUI::ViewController::AnimationType::In);
        showBackButton = true;
        ProvideInitialViewControllers(configsViewController, nullptr, nullptr, nullptr, nullptr);
    }

    void AnyTextFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
        // TODO Check if configs are good to save
        // |  Run name sanitize
        // |  Check for duplicates (with current list and saved files)
        // |  Notify and return if issues

        saveConfigs();

        _parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
    }

}