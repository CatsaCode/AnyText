#include "ui/configsViewController.hpp"
#include "main.hpp"

#include "configs.hpp"
#include "ui/configTable.hpp"

#include "bsml/shared/BSML.hpp"

#include "custom-types/shared/delegate.hpp"

#include "GlobalNamespace/EulaDisplayViewController.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

#include "HMUI/ScrollView.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, ConfigsViewController);

namespace AnyText::UI {

    void ConfigsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(!firstActivation) return;

        configTableView = BSML::Lite::CreateCustomSourceList<ConfigTableView*>(transform, {0, 15}, {120, 45});

        // BSML fails to use the scrollbar I want
        // Mostly copied from https://github.com/bsq-ports/Quest-BSML/blob/4987a88698becce955e44301fd4d573fc9180c2f/src/BSML/TypeHandlers/CustomCellListTableDataHandler.cpp#L82-L100
        ScrollView* scrollView = configTableView->GetComponentInChildren<ScrollView*>();
        UnityW<TextPageScrollView> textPageScrollViewTemplate = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::EulaDisplayViewController*>()->First()->_textPageScrollView;
        UnityW<TextPageScrollView> textPageScrollView = Object::Instantiate(textPageScrollViewTemplate, scrollView->get_transform(), false);
        scrollView->_pageUpButton = textPageScrollView->_pageUpButton;
        scrollView->_pageUpButton->get_onClick()->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(static_cast<std::function<void()>>(std::bind(&ScrollView::PageUpButtonPressed, scrollView))));
        scrollView->_pageDownButton = textPageScrollView->_pageDownButton;
        scrollView->_pageDownButton->get_onClick()->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(static_cast<std::function<void()>>(std::bind(&ScrollView::PageDownButtonPressed, scrollView))));
        scrollView->_verticalScrollIndicator = textPageScrollView->_verticalScrollIndicator;
        RectTransform* scrollBarTransform = textPageScrollView->_verticalScrollIndicator->get_transform()->get_parent().cast<RectTransform>();
        scrollBarTransform->SetParent(configTableView->get_transform(), false);
        scrollBarTransform->set_anchorMin({1, 0});
        scrollBarTransform->set_anchorMax({1, 1});
        scrollBarTransform->set_offsetMin({0, 0});
        scrollBarTransform->set_offsetMax({8, 0});
        Object::DestroyImmediate(textPageScrollView->get_gameObject());

        if(configTableView->tableView) configTableView->tableView->ReloadData();
    }

}