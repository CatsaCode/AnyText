#include "configs.hpp"
#include "main.hpp"

#include <filesystem>

using namespace AnyText;

std::vector<Config> AnyText::configs;

void AnyText::loadConfigs() {
    PaperLogger.info("Fetching configs from path: '{}'", getAnyTextDir());
    std::filesystem::recursive_directory_iterator(getAnyTextDir());
}