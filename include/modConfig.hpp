#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(findStrings, std::vector<std::string>, "findStrings", {"ANYTEXT"});
    CONFIG_VALUE(replaceStrings, std::vector<std::string>, "replaceStrings", {"<color=green>ANYTEXT"});
)