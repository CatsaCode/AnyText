#pragma once

#include "TMPro/TMP_Text.hpp"

#include <map>

namespace AnyText {

    extern std::map<std::string, std::string> findReplaceStrings;

    void RevertText(TMPro::TMP_Text* text);
    void ReplaceText(TMPro::TMP_Text* text);

    void InstallStringReplacerHooks();

}