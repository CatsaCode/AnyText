#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "TMPro/TMP_Text.hpp"

#ifdef BS_1_37_0
DECLARE_CLASS_CODEGEN(AnyText, TextManager, UnityEngine::MonoBehaviour,
#else
DECLARE_CLASS_CODEGEN(AnyText, TextManager, UnityEngine::MonoBehaviour) {
#endif

    DECLARE_INSTANCE_FIELD(UnityW<TMPro::TMP_Text>, text);

    DECLARE_INSTANCE_FIELD(StringW, prevTextStr);
    DECLARE_INSTANCE_FIELD(TMPro::FontStyles, prevFontStyle);
    DECLARE_INSTANCE_FIELD(bool, prevRichText);

    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDisale);

    DECLARE_INSTANCE_METHOD(void, SaveText);
    DECLARE_INSTANCE_METHOD(void, RestoreText);
    DECLARE_INSTANCE_METHOD(void, ReplaceText);

#ifdef BS_1_37_0
)
#else
};
#endif