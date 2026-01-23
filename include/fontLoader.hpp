#pragma once

#include "TMPro/TMP_FontAsset.hpp"

#include <map>

namespace AnyText {

    extern std::map<std::string, SafePtrUnity<TMPro::TMP_FontAsset>> fontAssets;

    // Name of the TMP_FontAsset that Beat Saber uses
    static constexpr std::string baseFontName = "Teko-Medium SDF";

    UnityEngine::Material* getFontAssetMaterial();
    UnityEngine::Material* getTextMaterial();
    
    void loadFonts();

}