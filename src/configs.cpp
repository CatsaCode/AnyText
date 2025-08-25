#include "configs.hpp"
#include "main.hpp"

using namespace AnyText;

std::vector<Config> AnyText::configs;

void AnyText::loadConfigs() {
    PaperLogger.info("Fetching configs from path: '{}'", getAnyTextDir());
}