#include "main.hpp"
#include "modConfig.hpp"

#include "scotland2/shared/modloader.h"

#include "bsml/shared/BSML.hpp"

#include "ui.hpp"
#include "stringReplacer.hpp"

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

// Called at the early stages of game loading
MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
    *info = modInfo.to_c();

    getModConfig().Init(modInfo);

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

    AnyText::InstallStringReplacerHooks();

    PaperLogger.info("Installed all hooks!");
}