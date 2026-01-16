#pragma once

#include "TMPro/TMP_FontAsset.hpp"

#include <map>

namespace AnyText {

    extern std::map<std::string, SafePtrUnity<TMPro::TMP_FontAsset>> fontAssets;

    void loadFonts();

}