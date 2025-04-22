#include "ui.hpp"
#include "main.hpp"

#include "stringReplacer.hpp"

#include "bsml/shared/BSML.hpp"

#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

namespace AnyText {

    UnityW<RectTransform> mainColumnTransform;

    void RegenerateFindReplaceStrings() {
        if(!mainColumnTransform) return;

        findReplaceStrings.clear();
        for(int i = 0; i < mainColumnTransform->get_childCount() - 1; i++) {
            std::string findString = mainColumnTransform->GetChild(i)->GetChild(0)->GetComponent<InputFieldView*>()->get_text()->ToLower();
            std::string replaceString = mainColumnTransform->GetChild(i)->GetChild(1)->GetComponent<InputFieldView*>()->get_text();
            findReplaceStrings[findString] = replaceString;
        }
    }

    GameObject* CreateFindReplaceSetting(Transform* parent, std::string_view findStartValue = "", std::string_view replaceStartValue = "") {
        GameObject* rowGO = GameObject::New_ctor("FindReplaceSetting");
        RectTransform* rowTransform = rowGO->AddComponent<RectTransform*>();
        rowTransform->SetParent(parent, false);
        rowTransform->SetSiblingIndex(parent->get_childCount() - 2);
        HorizontalLayoutGroup* rowLayoutG = rowGO->AddComponent<HorizontalLayoutGroup*>();
        rowLayoutG->set_childForceExpandWidth(false);
        rowLayoutG->set_childForceExpandHeight(false);
        rowLayoutG->set_childAlignment(TextAnchor::MiddleCenter);
        ContentSizeFitter* rowFitter = rowGO->AddComponent<ContentSizeFitter*>();
        rowFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        rowFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);

        InputFieldView* findStringSetting = BSML::Lite::CreateStringSetting(rowTransform, "Find", findStartValue, nullptr);
        LayoutElement* findStringSettingLayoutE = findStringSetting->GetComponent<LayoutElement*>();
        findStringSettingLayoutE->set_preferredWidth(50);
        
        InputFieldView* replaceStringSetting = BSML::Lite::CreateStringSetting(rowTransform, "Replace", replaceStartValue, nullptr);
        LayoutElement* replaceStringSettingLayoutE = replaceStringSetting->GetComponent<LayoutElement*>();
        replaceStringSettingLayoutE->set_preferredWidth(50);

        Button* removeButton = BSML::Lite::CreateUIButton(rowTransform, "X", [](){});
        LayoutElement* removeButtonLayoutE = removeButton->GetComponent<LayoutElement*>();
        removeButtonLayoutE->set_preferredWidth(6);

        return rowGO;
    }

    void RegenerateFindReplaceSettings() {
        if(!mainColumnTransform) return;

        for(int i = mainColumnTransform->get_childCount() - 2; i >= 0; i--) Object::DestroyImmediate(mainColumnTransform->GetChild(i)->get_gameObject());
        for(auto pair : findReplaceStrings) CreateFindReplaceSetting(mainColumnTransform, pair.first, pair.second);
    }

    void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(firstActivation) {
            std::function<void(bool, bool)> ModConfigMenuDidDeactivate = [](bool removedFromHierarchy, bool screenSystemDisabling){
                RegenerateFindReplaceStrings();
            };
            self->add_didDeactivateEvent(custom_types::MakeDelegate<HMUI::ViewController::DidDeactivateDelegate*>(ModConfigMenuDidDeactivate));

            VerticalLayoutGroup* mainColumn = BSML::Lite::CreateScrollableSettingsContainer(self->get_transform())->GetComponent<VerticalLayoutGroup*>();
            mainColumn->set_name("AnyTextMenu");
            mainColumn->set_spacing(-1.5);
            mainColumnTransform = mainColumn->get_rectTransform();
            
            Button* addButton = BSML::Lite::CreateUIButton(mainColumnTransform, "+", [](){CreateFindReplaceSetting(mainColumnTransform);});
            LayoutElement* addButtonLayoutE = addButton->GetComponent<LayoutElement*>();
            addButtonLayoutE->set_preferredWidth(6);
        }

        RegenerateFindReplaceSettings();
    }

}
