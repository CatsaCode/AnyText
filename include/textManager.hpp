#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "TMPro/TMP_Text.hpp"

namespace AnyText {
    struct TextState {
        std::string text;
        TMPro::FontStyles fontStyle;
        bool richText;
    };
}

#ifdef BS_1_37_0
DECLARE_CLASS_CODEGEN(AnyText, TextManager, UnityEngine::MonoBehaviour,
#else
DECLARE_CLASS_CODEGEN(AnyText, TextManager, UnityEngine::MonoBehaviour) {
#endif

    DECLARE_INSTANCE_FIELD(UnityW<TMPro::TMP_Text>, text);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
    DECLARE_INSTANCE_METHOD(void, OnTextChange);

    private:
        AnyText::TextState originalState;
        AnyText::TextState replacementState;

        bool updateOriginalStateWithDifferences();
        void applyState(const AnyText::TextState& state);
        void generateReplacementState();
    
    public:
        const AnyText::TextState& getOriginalState() const;

#ifdef BS_1_37_0
)
#else
};
#endif