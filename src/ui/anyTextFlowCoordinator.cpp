#include "ui/anyTextFlowCoordinator.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "ui/configsViewController.hpp"

#include "bsml/shared/Helpers/creation.hpp"
#include "bsml/shared/Helpers/getters.hpp"

DEFINE_TYPE(AnyText::UI, AnyTextFlowCoordinator);

namespace AnyText::UI {

    AnyTextFlowCoordinator* AnyTextFlowCoordinator::GetInstance() {
        static UnityW<AnyTextFlowCoordinator> instance;
        if(!instance) instance = BSML::Helpers::CreateFlowCoordinator<AnyTextFlowCoordinator*>();
        return instance;
    }

    void AnyTextFlowCoordinator::Present() {
        BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf()->PresentFlowCoordinator(GetInstance(), nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false, false);
    }


    void AnyTextFlowCoordinator::Awake() {
        if(!configsViewController) configsViewController = BSML::Helpers::CreateViewController<ConfigsViewController*>();
        if(!entriesViewController) entriesViewController = BSML::Helpers::CreateViewController<EntriesViewController*>();
    }

    void AnyTextFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(!firstActivation) return;

        SetTitle("AnyText", HMUI::ViewController::AnimationType::In);
        showBackButton = true;
        ProvideInitialViewControllers(configsViewController, nullptr, nullptr, nullptr, nullptr);
    }

    void AnyTextFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
        if(presentingEntries) {
            presentConfigs();
            return;
        }

        // TODO Check if configs are good to save
        // |  Run name sanitize
        // |  Check for duplicates (with current list and saved files)
        // |  Notify and return if issues

        saveConfigs();

        _parentFlowCoordinator->DismissFlowCoordinator(this, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
    }

    void AnyTextFlowCoordinator::presentEntries(Config* config) {
        presentingEntries = true;
        SetTitle(config->name, HMUI::ViewController::AnimationType::In);
        PresentViewController(entriesViewController, nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false);
        entriesViewController->setConfig(config);
    }

    void AnyTextFlowCoordinator::presentConfigs() {
        presentingEntries = false;
        SetTitle("AnyText", HMUI::ViewController::AnimationType::Out);
        DismissViewController(entriesViewController, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
    }

}