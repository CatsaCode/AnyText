#include "main.hpp"
#include "modConfig.hpp"

#include "configs.hpp"
#include "stringReplacer.hpp"
#include "textIdentifier.hpp"
#include "ui.hpp"
#include "ui/anyTextFlowCoordinator.hpp"

#include "scotland2/shared/modloader.h"
#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSML.hpp"

#include <filesystem>

modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

std::string_view getAnyTextDir() {
    static std::string anyTextDir = getDataDir("AnyText");
    if(!std::filesystem::exists(anyTextDir)) std::filesystem::create_directory(anyTextDir);
    return anyTextDir;
};

// Called at the early stages of game loading
MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
    *info = modInfo.to_c();

    Paper::Logger::RegisterFileContextId(PaperLogger.tag);

    getModConfig().Init(modInfo);

    AnyText::loadConfigs();

    PaperLogger.info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
MOD_EXTERN_FUNC void late_load() noexcept {
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    BSML::Init();
    BSML::Register::RegisterMenuButton("AnyText", "Change any text in the game", AnyText::UI::AnyTextFlowCoordinator::Present);

    PaperLogger.info("Installing hooks...");

    // AnyText::InstallStringReplacerHooks();
    AnyText::installTextIdentifierHooks();

    PaperLogger.info("Installed all hooks!");
}