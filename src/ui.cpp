#include "ui.hpp"
#include "main.hpp"
#include "modConfig.hpp"

#include "stringReplacer.hpp"

#include "bsml/shared/BSML.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

namespace AnyText {

    UnityW<RectTransform> mainColumnTransform;

    void UpdateMapConfigAndAllText() {
        if(!mainColumnTransform) return;

        std::vector<std::string> findStrings = {};
        std::vector<std::string> replaceStrings = {};
        findReplaceStrings.clear();
        for(int i = 0; i < mainColumnTransform->get_childCount() - 1; i++) {
            StringW findString = mainColumnTransform->GetChild(i)->GetChild(0)->GetComponent<InputFieldView*>()->get_text();
            StringW replaceString = mainColumnTransform->GetChild(i)->GetChild(1)->GetComponent<InputFieldView*>()->get_text();

            if(findString->get_Length() == 0) continue;

            findStrings.push_back(findString);
            replaceStrings.push_back(replaceString);
            findReplaceStrings[findString->ToLower()] = std::string(replaceString);
        }
        getModConfig().findStrings.SetValue(findStrings);
        getModConfig().replaceStrings.SetValue(replaceStrings);

        for(TMPro::TMP_Text* text : Resources::FindObjectsOfTypeAll<TMPro::TMP_Text*>()) {
            RevertText(text);
            ReplaceText(text);
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
        findStringSettingLayoutE->set_preferredWidth(40);
        for(CurvedTextMeshPro* findStringSettingText : findStringSetting->GetComponentsInChildren<CurvedTextMeshPro*>(true)) {
            findStringSettingText->set_richText(false);
        }
        
        InputFieldView* replaceStringSetting = BSML::Lite::CreateStringSetting(rowTransform, "Replace", replaceStartValue, nullptr);
        LayoutElement* replaceStringSettingLayoutE = replaceStringSetting->GetComponent<LayoutElement*>();
        replaceStringSettingLayoutE->set_preferredWidth(70);
        for(CurvedTextMeshPro* replaceStringSettingText : replaceStringSetting->GetComponentsInChildren<CurvedTextMeshPro*>(true)) {
            replaceStringSettingText->set_richText(false);
        }

        Button* removeButton = BSML::Lite::CreateUIButton(rowTransform, "X", [rowGO](){
            Object::Destroy(rowGO);
        });
        LayoutElement* removeButtonLayoutE = removeButton->GetComponent<LayoutElement*>();
        removeButtonLayoutE->set_preferredWidth(6);

        return rowGO;
    }

    void RegenerateFindReplaceSettings() {
        if(!mainColumnTransform) return;

        for(int i = mainColumnTransform->get_childCount() - 2; i >= 0; i--) {
            Object::DestroyImmediate(mainColumnTransform->GetChild(i)->get_gameObject());
        }

        std::vector<std::string> findStrings = getModConfig().findStrings.GetValue();
        std::vector<std::string> replaceStrings = getModConfig().replaceStrings.GetValue();
        for(int i = 0; i < std::min(findStrings.size(), replaceStrings.size()); i++) {
            CreateFindReplaceSetting(mainColumnTransform, findStrings[i], replaceStrings[i]);
        }
    }

    void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(firstActivation) {
            std::function<void(bool, bool)> ModConfigMenuDidDeactivate = [](bool removedFromHierarchy, bool screenSystemDisabling){
                UpdateMapConfigAndAllText();
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
