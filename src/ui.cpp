#include "ui.hpp"
#include "main.hpp"
#include "modConfig.hpp"

#include "stringReplacer.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/getters.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

#include "TMPro/TextMeshProUGUI.hpp"

#include "HMUI/ButtonStaticAnimations.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

namespace AnyText {

    UnityW<RectTransform> mainColumnTransform;

    void UpdateMapConfigAndAllText() {
        if(!mainColumnTransform) return;

        std::vector<std::pair<std::string, std::string>> configPairs = {};
        findReplaceStrings.clear();
        for(int i = 0; i < mainColumnTransform->get_childCount() - 1; i++) {
            StringW findString = mainColumnTransform->GetChild(i)->GetChild(0)->GetComponent<InputFieldView*>()->get_text();
            StringW replaceString = mainColumnTransform->GetChild(i)->GetChild(1)->GetComponent<InputFieldView*>()->get_text();

            if(findString->get_Length() == 0) continue;

            configPairs.push_back({findString, replaceString});
            findReplaceStrings[findString->ToLower()] = std::string(replaceString);
        }
        std::sort(configPairs.begin(), configPairs.end(), [](std::pair<std::string, std::string> a, std::pair<std::string, std::string> b){
            StringW aFirst = a.first;
            StringW bFirst = b.first;
            aFirst = aFirst->ToLower();
            bFirst = bFirst->ToLower();
            return aFirst->CompareTo(bFirst) <= 0;
        });
        auto uniqueIt = std::unique(configPairs.begin(), configPairs.end(), [](std::pair<std::string, std::string> a, std::pair<std::string, std::string> b){
            StringW aFirst = a.first;
            StringW bFirst = b.first;
            aFirst = aFirst->ToLower();
            bFirst = bFirst->ToLower();
            return aFirst->Equals(bFirst);
        });
        std::vector<std::string> findStrings = {};
        std::vector<std::string> replaceStrings = {};
        for(auto it = configPairs.begin(); it < uniqueIt; it++) {
            findStrings.push_back(it->first);
            replaceStrings.push_back(it->second);
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
            // Move container upwards so the keyboard doesn't clip into the ground
            RectTransform* scrollableSettingsContainerTransform = mainColumnTransform->get_parent()->get_parent()->get_parent()->GetComponent<RectTransform*>();
            scrollableSettingsContainerTransform->set_anchoredPosition(Vector2::op_Addition(scrollableSettingsContainerTransform->get_anchoredPosition(), Vector2(0, 7)));
            
            Button* addButton = BSML::Lite::CreateUIButton(mainColumnTransform, "+", [](){CreateFindReplaceSetting(mainColumnTransform);});
            LayoutElement* addButtonLayoutE = addButton->GetComponent<LayoutElement*>();
            addButtonLayoutE->set_preferredWidth(6);

            GameObject* infoCanvasGO = BSML::Lite::CreateCanvas();
            infoCanvasGO->set_name("AnyTextInfoCanvas");
            RectTransform* infoCanvasTransform = infoCanvasGO->GetComponent<RectTransform*>();
            infoCanvasTransform->set_anchorMin({0, 0});
            infoCanvasTransform->set_anchorMax({0, 0});
            infoCanvasTransform->set_pivot({0, 0});
            infoCanvasTransform->set_anchoredPosition({0, 0});
            infoCanvasTransform->set_sizeDelta({100, 100});
            infoCanvasTransform->set_position({-3.2, 0.1, 2.3});
            infoCanvasTransform->set_rotation(Quaternion::Euler({10, -45, 0}));

            GameObject* infoTextGO = GameObject::New_ctor("AnyTextInfo");
            RectTransform* infoTextTransform = infoTextGO->AddComponent<RectTransform*>();
            infoTextTransform->SetParent(infoCanvasTransform, false);
            infoTextTransform->set_anchorMin({0, 0});
            infoTextTransform->set_anchorMax({0, 0});
            infoTextTransform->set_pivot({0, 0});
            infoTextTransform->set_anchoredPosition({0, 0});
            infoTextTransform->set_sizeDelta({56, 0});
            TMPro::TextMeshProUGUI* infoText = infoTextGO->AddComponent<TMPro::TextMeshProUGUI*>();
            infoText->set_font(BSML::Helpers::GetMainTextFont());
            infoText->set_fontSharedMaterial(BSML::Helpers::GetMainUIFontMaterial());
            infoText->set_fontSize(2.45);
            infoText->set_color({1, 1, 1, 1});
            infoText->set_verticalAlignment(TMPro::VerticalAlignmentOptions::Bottom);
            infoText->set_text("\
<size=200%><b>Information</b></size><br>\
- Search term must match <u>fully</u>.<br>\
    \"a\" -> \"@\" will not replace every letter, just the key on the keyboard.<br>\
- Search term is <u>not case sensitive</u>.<br>\
- Search term is <u>indiscriminate</u>.<br>\
  Even song titles can be replaced if they happen to match.<br>\
- Emojis can be added from the config file.<br>\
<br>\
Rich text is mostly supported. The MoreButtons mod is recommended.<br>\
Examples<br>\
- <noparse><i></noparse><i>Italics</i><noparse></i></noparse> and normal<br>\
- <noparse><b></noparse><b>Bold</b><br>\
- <noparse><u></noparse><u>Underline</u><br>\
- <noparse><size=75%></noparse><size=75%>Font size</size><br>\
- <noparse><color=red></noparse><color=red>Color</color><br>\
- <noparse><color=#3beb8a></noparse><color=#3beb8a>Hex color</color><br>\
- <noparse><rotate=20></noparse><rotate=20>Rotate</rotate><br>\
- <noparse><nobr></noparse> Stops line breaks, which is useful for hit scores<br>\
For more info, visit <color=#3366cc>https://docs.unity3d.com/Packages/com.unity.ugui@2.0/manual/TextMeshPro/RichText.html</color><br>\
Unsupported tags include font, font-weight, gradient, link, page, sprite, and style.<br>\
<br>\
Have fun customizing! =D"\
            );

            Button* richTextLinkButton = BSML::Lite::CreateUIButton(infoCanvasTransform, "", [](){
                static auto OpenURL = il2cpp_utils::resolve_icall<void, StringW>("UnityEngine.Application::OpenURL");
                OpenURL("https://docs.unity3d.com/Packages/com.unity.ugui@2.0/manual/TextMeshPro/RichText.html");
            });
            RectTransform* richTextLinkButtonTransform = richTextLinkButton->GetComponent<RectTransform*>();
            Object::DestroyImmediate(richTextLinkButton->GetComponent<LayoutElement*>());
            Object::DestroyImmediate(richTextLinkButton->GetComponent<ContentSizeFitter*>());
            Object::Destroy(richTextLinkButtonTransform->Find("Content")->get_gameObject());
            Object::Destroy(richTextLinkButtonTransform->Find("Underline")->get_gameObject());
            Object::Destroy(richTextLinkButton->GetComponentInChildren<ButtonStaticAnimations*>());
            ImageView* richTextLinkButtonImage = richTextLinkButton->GetComponentInChildren<ImageView*>();
            richTextLinkButtonImage->_skew = 0;
            richTextLinkButtonImage->set_color({0, 0, 0, 0});
            richTextLinkButtonTransform->set_anchorMin({0, 0});
            richTextLinkButtonTransform->set_anchorMax({0, 0});
            richTextLinkButtonTransform->set_pivot({0, 0});
            richTextLinkButtonTransform->set_anchoredPosition({-1, 13.2});
            richTextLinkButtonTransform->set_sizeDelta({57, 9});

        }

        RegenerateFindReplaceSettings();
    }

}
