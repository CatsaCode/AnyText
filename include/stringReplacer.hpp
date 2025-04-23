#pragma once

#include "TMPro/TMP_Text.hpp"

#include <map>

namespace AnyText {

    extern std::map<std::string, std::string> findReplaceStrings;

    bool RevertText(TMPro::TMP_Text* text);
    bool ReplaceText(TMPro::TMP_Text* text);

    void InstallStringReplacerHooks();

}