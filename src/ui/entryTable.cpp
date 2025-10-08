#include "ui/entryTable.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "HMUI/InputFieldView.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/Button.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, EntryTableCell);
DEFINE_TYPE(AnyText::UI, EntryTableView);

namespace AnyText::UI {

    EntryTableCell* EntryTableCell::create() {
        PaperLogger.info("Creating EntryTableCell...");

        GameObject* entryTableCellGO = GameObject::New_ctor("EntryTableCell");
        RectTransform* entryTableCellTransform = entryTableCellGO->AddComponent<RectTransform*>();
        HorizontalLayoutGroup* entryTableCellHorizontal = entryTableCellGO->AddComponent<HorizontalLayoutGroup*>();
        entryTableCellHorizontal->set_spacing(2);
        ContentSizeFitter* entryTableCellFitter = entryTableCellGO->AddComponent<ContentSizeFitter*>();
        entryTableCellFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        entryTableCellFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
        EntryTableCell* entryTableCell = entryTableCellGO->AddComponent<EntryTableCell*>();

        GameObject* orderButtonsGO = GameObject::New_ctor("OrderButtons");
        RectTransform* orderButtonsTransform = orderButtonsGO->AddComponent<RectTransform*>();
        orderButtonsTransform->SetParent(entryTableCellTransform, false);
        HorizontalLayoutGroup* orderButtonsHorizontal = orderButtonsGO->AddComponent<HorizontalLayoutGroup*>();
        ContentSizeFitter* orderButtonsFitter = orderButtonsGO->AddComponent<ContentSizeFitter*>();
        orderButtonsFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);

        entryTableCell->upButton = BSML::Lite::CreateUIButton(orderButtonsTransform, "▲", std::bind(&EntryTableCell::HandleUpButtonOnClick, entryTableCell));
        entryTableCell->upButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);
        
        entryTableCell->downButton = BSML::Lite::CreateUIButton(orderButtonsTransform, "▼", std::bind(&EntryTableCell::HandleDownButtonOnClick, entryTableCell));
        entryTableCell->downButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);

        GameObject* mainSectionGO = GameObject::New_ctor("MainSection");
        RectTransform* mainSectionTransform = mainSectionGO->AddComponent<RectTransform*>();
        mainSectionTransform->SetParent(entryTableCellTransform, false);
        HorizontalLayoutGroup* mainSectionHorizontal = mainSectionGO->AddComponent<HorizontalLayoutGroup*>();
        ContentSizeFitter* mainSectionFitter = mainSectionGO->AddComponent<ContentSizeFitter*>();
        mainSectionFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);

        entryTableCell->findSettingsButton = BSML::Lite::CreateUIButton(mainSectionTransform, "⛭", std::bind(&EntryTableCell::HandleFindSettingsButtonOnClick, entryTableCell));
        entryTableCell->findSettingsButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);

        entryTableCell->findStringInput = BSML::Lite::CreateStringSetting(mainSectionTransform, "Find", "", std::bind(&EntryTableCell::HandleFindStringInputOnChange, entryTableCell));
        entryTableCell->findStringInput->_clearSearchButton->get_gameObject()->SetActive(false);
        entryTableCell->findStringInput->GetComponent<LayoutElement*>()->set_preferredWidth(40);

        entryTableCell->replaceStringInput = BSML::Lite::CreateStringSetting(mainSectionTransform, "Replace", "", std::bind(&EntryTableCell::HandleReplaceStringInputOnChange, entryTableCell));
        entryTableCell->replaceStringInput->_clearSearchButton->get_gameObject()->SetActive(false);
        entryTableCell->replaceStringInput->GetComponent<LayoutElement*>()->set_preferredWidth(40);

        entryTableCell->replaceSettingsButton = BSML::Lite::CreateUIButton(mainSectionTransform, "⛭", std::bind(&EntryTableCell::HandleReplaceSettingsButtonOnClick, entryTableCell));
        entryTableCell->replaceSettingsButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);

        entryTableCell->replaceSettingsButton = BSML::Lite::CreateUIButton(entryTableCellTransform, "X", std::bind(&EntryTableCell::HandleRemoveButtonOnClick, entryTableCell));
        entryTableCell->replaceSettingsButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);

        return entryTableCell;
    }

    void EntryTableCell::updateData(FindReplaceEntry* entry) {
        this->entry = entry;

        findStringInput->set_text(entry->findString);
    }

    void EntryTableCell::HandleUpButtonOnClick() {
        PaperLogger.info(__func__);
    }

    void EntryTableCell::HandleDownButtonOnClick() {
        PaperLogger.info(__func__);
    }

    void EntryTableCell::HandleFindSettingsButtonOnClick() {
        PaperLogger.info(__func__);
    }

    void EntryTableCell::HandleFindStringInputOnChange() {
        PaperLogger.info(__func__);
    }

    void EntryTableCell::HandleReplaceStringInputOnChange() {
        PaperLogger.info(__func__);
    }

    void EntryTableCell::HandleReplaceSettingsButtonOnClick() {
        PaperLogger.info(__func__);
    }

    void EntryTableCell::HandleRemoveButtonOnClick() {
        PaperLogger.info(__func__);
    }

    void EntryTableCell::WasPreparedForReuse() {
        entry = nullptr;
    }



    float EntryTableView::CellSize() {
        return cellSize;
    }

    int EntryTableView::NumberOfCells() {
        if(!config) return 0;
        return config->entries.size();
    }

    HMUI::TableCell* EntryTableView::CellForIdx(HMUI::TableView* tableView, int idx) {
        this->tableView = tableView;
        UnityW<EntryTableCell> entryTableCell = static_cast<UnityW<EntryTableCell>>(tableView->DequeueReusableCellForIdentifier(reuseIdentifier));
        if(!entryTableCell) {
            entryTableCell = EntryTableCell::create();
            entryTableCell->reuseIdentifier = reuseIdentifier;
        }
        entryTableCell->updateData(&config->entries[idx]);
        return entryTableCell;
    }

}