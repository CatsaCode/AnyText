#include "main.hpp"

#include "scotland2/shared/modloader.h"

// #include "GlobalNamespace/StandardLevelDetailView.hpp"
// #include "GlobalNamespace/StandardLevelDetailViewController.hpp"
// #include "UnityEngine/UI/Button.hpp"
// #include "UnityEngine/GameObject.hpp"
// #include "HMUI/CurvedTextMeshPro.hpp"

#include "TMPro/TextMeshPro.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

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
    self->set_richText(true);
    self->set_text("<color=#00ff00>AnyText");

    TextMeshProHook(self);
}

MAKE_HOOK_MATCH(TextMeshProUGUIHook, &TMPro::TextMeshProUGUI::SetVerticesDirty, void, 
TMPro::TextMeshProUGUI* self) {
    
    // Used for everything else
    self->set_richText(true);
    self->set_text("<size=0>Hehe secret text :3</size>AnyText");

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

    PaperLogger.info("Installing hooks...");

    INSTALL_HOOK(PaperLogger, TextMeshProHook);
    INSTALL_HOOK(PaperLogger, TextMeshProUGUIHook)

    PaperLogger.info("Installed all hooks!");
}