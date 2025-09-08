#include "ui/configsViewController.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "bsml/shared/BSML.hpp"

#include "custom-types/shared/delegate.hpp"

#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, ConfigsViewController);

namespace AnyText::UI {

    static UnityW<VerticalLayoutGroup> configsVertical;

    static GameObject* createConfigSetting(Transform* parent, Config& config) {
        PaperLogger.info("Creating config: '{}'", config.name);
        GameObject* configSettingGO = GameObject::New_ctor("ConfigSetting");
        RectTransform* configSettingTransform = configSettingGO->AddComponent<RectTransform*>();
        configSettingTransform->SetParent(parent, false);
        configSettingTransform->SetSiblingIndex(parent->get_childCount() - 2);
        HorizontalLayoutGroup* configSettingHorizontal = configSettingGO->AddComponent<HorizontalLayoutGroup*>();
        configSettingHorizontal->set_spacing(2);
        ContentSizeFitter* configSettingFitter = configSettingGO->AddComponent<ContentSizeFitter*>();
        configSettingFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        configSettingFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);

        GameObject* orderButtonsGO = GameObject::New_ctor("OrderButtons");
        RectTransform* orderButtonsTransform = orderButtonsGO->AddComponent<RectTransform*>();
        orderButtonsTransform->SetParent(configSettingTransform, false);
        HorizontalLayoutGroup* orderButtonsHorizontal = orderButtonsGO->AddComponent<HorizontalLayoutGroup*>();
        ContentSizeFitter* orderButtonsFitter = orderButtonsGO->AddComponent<ContentSizeFitter*>();
        orderButtonsFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);

        Button* upButton = BSML::Lite::CreateUIButton(orderButtonsTransform, "Up");
        upButton->GetComponent<LayoutElement*>()->set_preferredWidth(10);
        
        Button* downButton = BSML::Lite::CreateUIButton(orderButtonsTransform, "Down", [](){PaperLogger.info("Down");});
        downButton->GetComponent<LayoutElement*>()->set_preferredWidth(10);
        
        InputFieldView* configNameInput = BSML::Lite::CreateStringSetting(configSettingTransform, "Config name", config.name, [&config](StringW value){PaperLogger.info("Updating config name '{}' to '{}'", config.name, value);});
        configNameInput->GetComponent<LayoutElement*>()->set_preferredWidth(70);
        
        Button* editButton = BSML::Lite::CreateUIButton(configSettingTransform, "Edit", [](){PaperLogger.info("Edit");});
        editButton->GetComponent<LayoutElement*>()->set_preferredWidth(12);
        Button* deleteButton = BSML::Lite::CreateUIButton(configSettingTransform, "X", [](){PaperLogger.info("Delete");});
        deleteButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);


        std::function<void()> updateOrderButtonInteractability = [&](){
            upButton->set_interactable(configSettingTransform->GetSiblingIndex() > 0);
            downButton->set_interactable(configSettingTransform->GetSiblingIndex() < configSettingTransform->GetParent()->GetChildCount() - 2);
        };
        updateOrderButtonInteractability();

        // TODO
        // Order button interactibility needs to be processed in configsViewController. Each item is the last item when it is first created, resulting in all the down buttons being grayed out
        // Order button interactivility function is likely being destroyed and the delegate callback tries calling a dangling pointer

        upButton->get_onClick()->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(std::function<void()>([&, updateOrderButtonInteractability](){
            int currentIndex = configSettingTransform->GetSiblingIndex();
            
            Config tmpConfig = configs[currentIndex];
            configs[currentIndex - 1] = configs[currentIndex];
            configs[currentIndex] = tmpConfig;
            saveConfigOrder();

            configSettingTransform->SetSiblingIndex(configSettingTransform->GetSiblingIndex() - 1);
            updateOrderButtonInteractability();
        })));

        downButton->get_onClick()->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(std::function<void()>([&, updateOrderButtonInteractability](){
            int currentIndex = configSettingTransform->GetSiblingIndex();
            
            Config tmpConfig = configs[currentIndex];
            configs[currentIndex + 1] = configs[currentIndex];
            configs[currentIndex] = tmpConfig;
            saveConfigOrder();

            configSettingTransform->SetSiblingIndex(configSettingTransform->GetSiblingIndex() + 1);
            updateOrderButtonInteractability();
        })));


        return configSettingGO;
    }

    void ConfigsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(!firstActivation) return;

        configsVertical = BSML::Lite::CreateScrollableSettingsContainer(transform)->GetComponent<VerticalLayoutGroup*>();
        configsVertical->set_name("ConfigsVertical");
        configsVertical->set_spacing(-1.5);
        configsVerticalTransform = configsVertical->get_rectTransform();
        // Move container upwards so the keyboard doesn't clip into the ground so much
        RectTransform* scrollableSettingsContainerTransform = configsVerticalTransform->get_parent()->get_parent()->get_parent()->GetComponent<RectTransform*>();
        scrollableSettingsContainerTransform->set_anchoredPosition(Vector2::op_Addition(scrollableSettingsContainerTransform->get_anchoredPosition(), Vector2(0, 7)));
        
        Button* addButton = BSML::Lite::CreateUIButton(configsVerticalTransform, "+", [](){PaperLogger.debug("DEBUG Adding new config...");});
        LayoutElement* addButtonLayoutE = addButton->GetComponent<LayoutElement*>();
        addButtonLayoutE->set_preferredWidth(6);

        for(Config& config : configs) createConfigSetting(configsVerticalTransform, config);
    }

}