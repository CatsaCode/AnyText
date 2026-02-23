#pragma once

#include "main.hpp"

#include "config-utils/shared/config-utils.hpp"

#include <vector>

namespace AnyText {

    #ifdef BS_1_37_0
    DECLARE_CONFIG(ModConfig,
    #else
    DECLARE_CONFIG(ModConfig) {
    #endif
        CONFIG_VALUE(configOrder, std::vector<std::string>, "configOrder", {});
        CONFIG_VALUE(disabledConfigs, std::vector<std::string>, "disabledConfigs", {});
        KEEP_EXTRA_FIELDS;
        
        DESERIALIZE_FUNCTION(upgradeVersion);
        
    #ifdef BS_1_37_0
    );
    #else
    };
    #endif

}