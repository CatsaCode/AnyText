#pragma once

#include "config-utils/shared/config-utils.hpp"

#ifdef BS_1_37_0
DECLARE_CONFIG(ModConfig,
#else
DECLARE_CONFIG(ModConfig) {
#endif
    
    CONFIG_VALUE(findStrings, std::vector<std::string>, "findStrings", {"ANYTEXT"});
    CONFIG_VALUE(replaceStrings, std::vector<std::string>, "replaceStrings", {"<color=green>ANYTEXT"});
    // KEEP_EXTRA_FIELDS;

    CONFIG_VALUE(configOrder, std::vector<std::string>, "configOrder", {});
    
#ifdef BS_1_37_0
);
#else
};
#endif