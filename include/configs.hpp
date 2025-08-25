#pragma once

#include "config-utils/shared/config-utils.hpp"

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

    DECLARE_JSON_STRUCT(Config) {
        VALUE_DEFAULT(int, version, 0);
        VALUE_DEFAULT(std::vector<FindReplaceEntry>, entries, {});
    };

    extern std::vector<Config> configs;
    void loadConfigs();

}