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
        if(!entry) {PaperLogger.error("entry is nullptr"); return;}
        this->entry = entry;

        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}
        std::vector<FindReplaceEntry>& entries = entryTableView->config->entries;
        int index = std::distance(entries.begin(), std::find_if(entries.begin(), entries.end(), [this](const FindReplaceEntry& x){return &x == this->entry;}));
        if(index == entries.size()) {PaperLogger.error("entry not found in entries vector"); return;}
        upButton->set_interactable(index > 0);
        downButton->set_interactable(index < entries.size() - 1);

        findStringInput->set_text(entry->findString);
        replaceStringInput->set_text(entry->replaceString);
    }

    void EntryTableCell::HandleUpButtonOnClick() {
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}
        std::vector<FindReplaceEntry>& entries = entryTableView->config->entries;

        int index = std::distance(entries.begin(), std::find_if(entries.begin(), entries.end(), [this](const FindReplaceEntry& x){return &x == this->entry;}));
        if(index == entries.size()) {PaperLogger.error("entry not found in entries vector"); return;}
        if(index <= 0) {PaperLogger.error("Can't move entry up, entry is already at top"); return;}
        
        PaperLogger.info("Entry: '{}', Index: {} -1, Idx: {}", entry->findString, index, idx);
        std::swap(entries[index], entries[index - 1]);
        MoveIdx(-1);

        entryTableView->ReloadEntryOrder();
    }

    void EntryTableCell::HandleDownButtonOnClick() {
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}
        std::vector<FindReplaceEntry>& entries = entryTableView->config->entries;

        int index = std::distance(entries.begin(), std::find_if(entries.begin(), entries.end(), [this](const FindReplaceEntry& x){return &x == this->entry;}));
        if(index == entries.size()) {PaperLogger.error("entry not found in entries vector"); return;}
        if(index >= entries.size() - 1) {PaperLogger.error("Can't move entry down, entry is already at bottom"); return;}
        
        PaperLogger.info("Entry: '{}', Index: {} +1, Idx: {}", entry->findString, index, idx);
        std::swap(entries[index], entries[index + 1]);
        MoveIdx(1);

        entryTableView->config->unsaved = true;
        entryTableView->ReloadEntryOrder();
    }

    void EntryTableCell::HandleFindSettingsButtonOnClick() {
        PaperLogger.info(__func__);
    }

    void EntryTableCell::HandleFindStringInputOnChange() {
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}

        entry->findString = std::string(findStringInput->get_text());
        entryTableView->config->unsaved = true;
    }

    void EntryTableCell::HandleReplaceStringInputOnChange() {
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}

        entry->replaceString = std::string(replaceStringInput->get_text());
        entryTableView->config->unsaved = true;
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
            entryTableCell->entryTableView = this;
            entryTableCell->reuseIdentifier = reuseIdentifier;
        }
        entryTableCell->updateData(&config->entries[idx]);
        return entryTableCell;
    }

    void EntryTableView::ReloadEntryOrder() {
        tableView->ReloadDataKeepingPosition();
    }

}