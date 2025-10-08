#include "ui/entryListViewController.hpp"
#include "main.hpp"

#include "configs.hpp"
#include "ui/entryTable.hpp"

#include "bsml/shared/BSML.hpp"

#include "custom-types/shared/delegate.hpp"

#include "GlobalNamespace/EulaDisplayViewController.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

#include "HMUI/ScrollView.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, EntryListViewController);

namespace AnyText::UI {

    void EntryListViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(!firstActivation) return;

        entryTableView = BSML::Lite::CreateCustomSourceList<EntryTableView*>(transform, {0, 15}, {120, 45});

        // BSML fails to use the scrollbar I want
        // Mostly copied from https://github.com/bsq-ports/Quest-BSML/blob/4987a88698becce955e44301fd4d573fc9180c2f/src/BSML/TypeHandlers/CustomCellListTableDataHandler.cpp#L82-L100
        ScrollView* scrollView = entryTableView->GetComponentInChildren<ScrollView*>();
        UnityW<TextPageScrollView> textPageScrollViewTemplate = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::EulaDisplayViewController*>()->First()->_textPageScrollView;
        UnityW<TextPageScrollView> textPageScrollView = Object::Instantiate(textPageScrollViewTemplate, scrollView->get_transform(), false);
        scrollView->_pageUpButton = textPageScrollView->_pageUpButton;
        scrollView->_pageUpButton->get_onClick()->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(static_cast<std::function<void()>>(std::bind(&ScrollView::PageUpButtonPressed, scrollView))));
        scrollView->_pageDownButton = textPageScrollView->_pageDownButton;
        scrollView->_pageDownButton->get_onClick()->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(static_cast<std::function<void()>>(std::bind(&ScrollView::PageDownButtonPressed, scrollView))));
        scrollView->_verticalScrollIndicator = textPageScrollView->_verticalScrollIndicator;
        RectTransform* scrollBarTransform = textPageScrollView->_verticalScrollIndicator->get_transform()->get_parent().cast<RectTransform>();
        scrollBarTransform->SetParent(entryTableView->get_transform(), false);
        scrollBarTransform->set_anchorMin({1, 0});
        scrollBarTransform->set_anchorMax({1, 1});
        scrollBarTransform->set_offsetMin({0, 0});
        scrollBarTransform->set_offsetMax({8, 0});
        Object::DestroyImmediate(textPageScrollView->get_gameObject());

        if(entryTableView->tableView) entryTableView->tableView->ReloadData();
    }

    void EntryListViewController::setConfig(Config* config) {
        if(!entryTableView) {PaperLogger.info("entryTableView is not assigned"); return;}
        entryTableView->config = config;
        entryTableView->tableView->ReloadData();
    }
    
}