#include "stringReplacer.hpp"
#include "main.hpp"
#include "modConfig.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"

#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Resources.hpp"

#include "TMPro/TMP_Text.hpp"

#include <string>

using namespace GlobalNamespace;
using namespace UnityEngine;

namespace AnyText {

    std::map<std::string, std::string> findReplaceStrings = {};
    //std::unordered_set<std::string> uniqueStrings = {};

    // void LogUniqueText(StringW text) {
    //     if(text->StartsWith("<size=0>AnyText_")) return;
    //     if(uniqueStrings.contains(text)) return;
        
    //     uniqueStrings.insert(text);
    //     PaperLogger.info("Detected text: \"{}\"", text); // DEBUG ABSOLUTELY DO NOT LEAVE THIS IN A RELEASE OR HUNDREDS OF PASSWORDS AND OTHER SENSITIVE DATA WILL BE LEAKED!!!
    // }

    bool RevertText(TMPro::TMP_Text* text) {
        if(!text ||
            !text->m_text ||
            !text->m_text->StartsWith("<size=0>AnyText_")
        ) {
            return false;
        }

        int underscore1 = 15;
        int underscore2 = text->m_text->IndexOf('_', underscore1 + 1);
        int underscore3 = text->m_text->IndexOf('_', underscore2 + 1);
        
        std::string originalStyleString = text->m_text->Substring(underscore1 + 1, underscore2 - underscore1 - 1);
        int32_t originalStyle = std::stoi(originalStyleString);
        std::string originalText = text->m_text->Substring(underscore2 + 1, underscore3 - underscore2 - 1);


        text->m_fontStyle = originalStyle;
        text->m_text = originalText;

        return true;
    }

    bool ReplaceText(TMPro::TMP_Text* text) {
        if(!text || !text->m_text) return false;

        Transform* menuTransform = text->get_transform();
        for(int i = 0; i < 4; i++) {
            if(!menuTransform) break;
            if(menuTransform->get_name() == "AnyTextMenu") return false;
            menuTransform = menuTransform->get_parent();
        }

        // LogUniqueText(text->m_text);

        std::string textKey = text->m_text->ToLower();
        // To-do Create algorithm to extract the rich text tags. GetParsedText() can not be relied upon because it uses m_textInfo which has not yet been created at this stage of making the text
        if(!findReplaceStrings.contains(textKey)) return false;
        

        // Private variables are set directly because the public setters will call SetVerticesDirty(), creating an infinite loop
        text->m_isRichText = true;

        // Remove the uppercase bit from the fontStyle
        int32_t oldStyle = static_cast<int32_t>(text->get_fontStyle());
        int32_t newStyle = oldStyle;
        int32_t upper = static_cast<int32_t>(TMPro::FontStyles::UpperCase);
        newStyle &= ~upper;
        text->m_fontStyle = newStyle;

        text->m_text = fmt::format("<size=0>AnyText_{}_{}_AnyText</size>{}", oldStyle, text->m_text, findReplaceStrings[textKey]);

        return true;
    }



    MAKE_HOOK_MATCH(InitialReplaceHook, &MainMenuViewController::DidActivate, void,
    MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        InitialReplaceHook(self, firstActivation, addedToHierarchy, screenSystemEnabling);

        if(!firstActivation) return;

        std::vector<std::string> findStrings = getModConfig().findStrings.GetValue();
        std::vector<std::string> replaceStrings = getModConfig().replaceStrings.GetValue();
        for(int i = 0; i < std::min(findStrings.size(), replaceStrings.size()); i++) {
            StringW key = findStrings[i];
            key = key->ToLower();
            findReplaceStrings[key] = replaceStrings[i];
        }

        for(TMPro::TMP_Text* text : Resources::FindObjectsOfTypeAll<TMPro::TMP_Text*>()) {
            if(ReplaceText(text)) text->SetAllDirty();
        }
    }

    MAKE_HOOK_MATCH(ReplaceOnParseHook, &TMPro::TMP_Text::ParseInputText, void,
    TMPro::TMP_Text* self) {
        ReplaceText(self);
        ReplaceOnParseHook(self);
    }

    void InstallStringReplacerHooks() {
        INSTALL_HOOK(PaperLogger, InitialReplaceHook);
        INSTALL_HOOK(PaperLogger, ReplaceOnParseHook);
    }

}