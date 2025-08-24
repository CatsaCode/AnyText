#pragma once

#include "config-utils/shared/config-utils.hpp"
#include "UnityEngine/Color.hpp"

namespace AnyText {

    // enum class FindAlgorithm {
    //     ExactMatch,
    //     PartialMatch,
    //     Regex
    // };

    // enum class FindLocation {
    //     MenuScene,
    //     GameScene
    // };

    DECLARE_JSON_STRUCT(FindReplaceEntry) {
        VALUE(std::string, findString);
        VALUE(std::string, replaceString);

        // VALUE_DEFAULT(FindAlgorithm, findAlgorithm, FindAlgorithm::PartialMatch);
        // VALUE_DEFAULT(bool, accumulate, false);
        // VALUE_OPTIONAL(FindLocation, findLocation);

        // VALUE_OPTIONAL(bool, replaceAll);
        // VALUE_OPTIONAL(bool, bold);
        // VALUE_OPTIONAL(bool, italics);
        // VALUE_OPTIONAL(bool, underline);
        // VALUE_OPTIONAL(bool, strikethrough);
        // VALUE_OPTIONAL(bool, lowercase);
        // VALUE_OPTIONAL(bool, uppercase);
        // VALUE_OPTIONAL(bool, smallcaps);
        // VALUE_OPTIONAL(int, fontSize);
        // VALUE_OPTIONAL(bool, autoSize);
        // Color
        // Gradient
        // Spacing options
        // Alignment
        // Wrapping
        // Overflow
    };

}

#ifdef BS_1_37_0
DECLARE_CONFIG(ModConfig,
#else
DECLARE_CONFIG(ModConfig) {
#endif
    
    CONFIG_VALUE(findStrings, std::vector<std::string>, "findStrings", {"ANYTEXT"});
    CONFIG_VALUE(replaceStrings, std::vector<std::string>, "replaceStrings", {"<color=green>ANYTEXT"});
    // KEEP_EXTRA_FIELDS;
    
#ifdef BS_1_37_0
);
#else
};
#endif