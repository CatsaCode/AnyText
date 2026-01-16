#include "main.hpp"
#include "modConfig.hpp"

#include "configs.hpp"
#include "fontLoader.hpp"
#include "textIdentifier.hpp"
#include "ui/anyTextFlowCoordinator.hpp"

#include "scotland2/shared/modloader.h"
#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSML.hpp"

#include "GlobalNamespace/MainFlowCoordinator.hpp"

#include <filesystem>

modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

std::string_view getAnyTextDir() {
    static std::string anyTextDir = getDataDir("AnyText");
    if(!std::filesystem::exists(anyTextDir)) std::filesystem::create_directory(anyTextDir);
    return anyTextDir;
};


MAKE_HOOK_MATCH(MainFlowCoordinator_DidActivate, &GlobalNamespace::MainFlowCoordinator::DidActivate, 
    void, GlobalNamespace::MainFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling
) {
    MainFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if(!firstActivation) return;

    AnyText::loadFonts();
    // Identify all text
}


// Called at the early stages of game loading
MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
    *info = modInfo.to_c();

    Paper::Logger::RegisterFileContextId(PaperLogger.tag);

    AnyText::getModConfig().Init(modInfo);

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

    INSTALL_HOOK(PaperLogger, MainFlowCoordinator_DidActivate);
    AnyText::installTextIdentifierHooks();

    PaperLogger.info("Installed all hooks!");
}