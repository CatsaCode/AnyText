#include "main.hpp"

#include "scotland2/shared/modloader.h"

#include "bsml/shared/BSML.hpp"

#include "ui.hpp"
#include "stringReplacer.hpp"

// #include "GlobalNamespace/StandardLevelDetailView.hpp"
// #include "GlobalNamespace/StandardLevelDetailViewController.hpp"
// #include "UnityEngine/UI/Button.hpp"
// #include "UnityEngine/GameObject.hpp"
// #include "HMUI/CurvedTextMeshPro.hpp"

#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include <map>

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};
// Stores the ID and version of our mod, and is sent to
// the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be
// removed if those are in use
Configuration &getConfig() {
    static Configuration config(modInfo);
    return config;
}



MAKE_HOOK_MATCH(TextMeshProHook, &TMPro::TextMeshPro::SetVerticesDirty, void, 
TMPro::TextMeshPro* self) {
    
    // Used for hit scores
    // self->set_richText(true);
    // self->set_text("<color=#00ff00>AnyText");

    TextMeshProHook(self);
}

MAKE_HOOK_MATCH(TextMeshProUGUIHook, &TMPro::TextMeshProUGUI::SetVerticesDirty, void, 
TMPro::TextMeshProUGUI* self) {
    
    // Used for everything else
    // self->set_richText(true);
    // self->set_text("<size=0>Hehe secret text :3</size>AnyText");

    // if(self->text) PaperLogger.info("Parsed: {}", self->GetParsedText());
    // if(self->text) self->text = self->text->ToLower();

    // int32_t style = static_cast<int32_t>(self->get_fontStyle());
    // int32_t upper = static_cast<int32_t>(TMPro::FontStyles::UpperCase);
    // style &= ~upper;
    // self->set_fontStyle(style);

    if(self->m_text) {
        std::string textKey = self->m_text->ToLower();
        // To-do Create algorithm to extract the rich text tags. GetParsedText() can not be relied upon because it uses m_textInfo which has not yet been created at this stage of making the text

        if(AnyText::findReplaceStrings.contains(textKey)) self->m_text = AnyText::findReplaceStrings[textKey];
    }

    TextMeshProUGUIHook(self);
}



// Called at the early stages of game loading
MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
    *info = modInfo.to_c();

    getConfig().Load();

    // File logging
    Paper::Logger::RegisterFileContextId(PaperLogger.tag);

    PaperLogger.info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
MOD_EXTERN_FUNC void late_load() noexcept {
    il2cpp_functions::Init();
    BSML::Init();
    BSML::Register::RegisterMainMenuViewControllerMethod("AnyText", "AnyText", "Change any text in the game", AnyText::DidActivate);

    PaperLogger.info("Installing hooks...");

    INSTALL_HOOK(PaperLogger, TextMeshProHook);
    INSTALL_HOOK(PaperLogger, TextMeshProUGUIHook)

    PaperLogger.info("Installed all hooks!");
}