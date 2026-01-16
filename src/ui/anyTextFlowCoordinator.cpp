#include "ui/anyTextFlowCoordinator.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "ui/configListViewController.hpp"

#include "bsml/shared/Helpers/creation.hpp"
#include "bsml/shared/Helpers/getters.hpp"

DEFINE_TYPE(AnyText::UI, AnyTextFlowCoordinator);

namespace AnyText::UI {

    UnityW<AnyTextFlowCoordinator> AnyTextFlowCoordinator::instance;

    void AnyTextFlowCoordinator::Present() {
        if(!AnyTextFlowCoordinator::instance) AnyTextFlowCoordinator::instance = BSML::Helpers::CreateFlowCoordinator<AnyTextFlowCoordinator*>();
        BSML::Helpers::GetMainFlowCoordinator()->YoungestChildFlowCoordinatorOrSelf()->PresentFlowCoordinator(AnyTextFlowCoordinator::instance, nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false, false);
    }


    void AnyTextFlowCoordinator::Awake() {
        if(!configListViewController) configListViewController = BSML::Helpers::CreateViewController<ConfigListViewController*>();
        if(!entryListViewController) entryListViewController = BSML::Helpers::CreateViewController<EntryListViewController*>();
    }

    void AnyTextFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(!firstActivation) return;

        SetTitle("AnyText", HMUI::ViewController::AnimationType::In);
        showBackButton = true;
        ProvideInitialViewControllers(configListViewController, nullptr, nullptr, nullptr, nullptr);
    }

    void AnyTextFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topViewController) {
        PaperLogger.debug("&AnyTextFlowCoordinator: {}", static_cast<void*>(this));
        
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
        PaperLogger.debug("&AnyTextFlowCoordinator: {}, &config: {}", static_cast<void*>(this), static_cast<void*>(config));
        presentingEntries = true;
        SetTitle(config->name, HMUI::ViewController::AnimationType::In);
        PresentViewController(entryListViewController, nullptr, HMUI::ViewController::AnimationDirection::Horizontal, false);
        entryListViewController->setConfig(config);
    }

    void AnyTextFlowCoordinator::presentConfigs() {
        PaperLogger.debug("&AnyTextFlowCoordinator: {}", static_cast<void*>(this));
        presentingEntries = false;
        SetTitle("AnyText", HMUI::ViewController::AnimationType::Out);
        DismissViewController(entryListViewController, HMUI::ViewController::AnimationDirection::Horizontal, nullptr, false);
    }

}