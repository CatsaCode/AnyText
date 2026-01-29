#include "textIdentifier.hpp"
#include "main.hpp"

#include "findReplaceEntry.hpp"
#include "configs.hpp"
#include "textManager.hpp"
#include "ui/anyTextFlowCoordinator.hpp"

#include "boost/regex.hpp"

#include "TMPro/TMP_Text.hpp"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/FindObjectsSortMode.hpp"
#include "UnityEngine/FindObjectsInactive.hpp"

using namespace UnityEngine;

namespace AnyText {

    void identifyText(TMPro::TMP_Text* text) {
        if(!text || !text->get_text() || !text->get_gameObject()) return;

        if( UI::AnyTextFlowCoordinator::instance &&
            ((UI::AnyTextFlowCoordinator::instance->configListViewController && 
            text->get_transform()->IsChildOf(UI::AnyTextFlowCoordinator::instance->configListViewController->get_transform())) ||
            (UI::AnyTextFlowCoordinator::instance->entryListViewController && 
            text->get_transform()->IsChildOf(UI::AnyTextFlowCoordinator::instance->entryListViewController->get_transform())))
        ) {
            return;
        }
        
        TextManager* textManager = text->GetComponent<TextManager*>();
        std::string identifyingText = textManager ? textManager->getOriginalState().text : std::string(text->get_text());

        for(Config& config : configs) {
            for(FindReplaceEntry& entry : config.entries) {
                if(!entry.getFindRegexIsValid()) continue;
                if(!boost::regex_search(identifyingText, entry.getFindRegex())) continue;

                if(!textManager) {
                    PaperLogger.debug("Adding TextManager to '{}'", identifyingText);
                    text->get_gameObject()->AddComponent<TextManager*>();
                }
                else {
                    PaperLogger.debug("Updating TextManager for '{}'", identifyingText);
                    textManager->OnTextChange();
                }

                return;
            }
        }

        // PaperLogger.debug("Could not match '{}'", identifyingText);
        if(textManager) {
            PaperLogger.debug("Destroying TextManager");
            Object::Destroy(textManager);
        }
    }

    void identifyAllText() {
        for(auto text : Object::FindObjectsByType<TMPro::TMP_Text*>(FindObjectsInactive::Include, FindObjectsSortMode::None)) {
            identifyText(text);
        }
    }

    void updateAllReplacements() {
        for(auto textManager : Object::FindObjectsByType<TextManager*>(FindObjectsInactive::Include, FindObjectsSortMode::None)) {
            textManager->UpdateReplacement();
        }
    }

    MAKE_HOOK_MATCH(TMP_Text_ParseInputText, &TMPro::TMP_Text::ParseInputText, 
        void, TMPro::TMP_Text* self
    ) {
        identifyText(self);
        TMP_Text_ParseInputText(self);
    }

    void installTextIdentifierHooks() {
        INSTALL_HOOK(PaperLogger, TMP_Text_ParseInputText);
    }

}