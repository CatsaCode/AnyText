#include "ui/entryTable.hpp"
#include "main.hpp"
#include "assets.hpp"

#include "findReplaceEntry.hpp"
#include "configs.hpp"
#include "ui/anyTextFlowCoordinator.hpp"
#include "ui/utils.hpp"

#include "custom-types/shared/delegate.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "GlobalNamespace/UIKeyboardManager.hpp"

#include "HMUI/InputFieldView.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/TouchScreenKeyboard.hpp"
#include "UnityEngine/TouchScreenKeyboardType.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/UI/Button.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, EntryTableCell);
DEFINE_TYPE(AnyText::UI, EntryCreatorTableCell);
DEFINE_TYPE(AnyText::UI, EntryTableView);

namespace AnyText::UI {

    static custom_types::Helpers::Coroutine openSystemKeyboard(InputFieldView* inputFieldView, TouchScreenKeyboardType type = TouchScreenKeyboardType::Default, std::string_view placeholder = "", int charLimit = 0) {
        static TouchScreenKeyboard* systemKeyboard = nullptr;
        if(!systemKeyboard) systemKeyboard = TouchScreenKeyboard::Open(inputFieldView->get_text(), type, true, false, false, false, placeholder, charLimit);
        
        float startTimeSeconds = Time::get_realtimeSinceStartup();
        while(
            systemKeyboard && 
            systemKeyboard->status != TouchScreenKeyboard::Status::Done &&
            systemKeyboard->status != TouchScreenKeyboard::Status::Canceled &&
            systemKeyboard->status != TouchScreenKeyboard::Status::LostFocus
        ) {
            co_yield nullptr;
        }
        float endTimeSeconds = Time::get_realtimeSinceStartup();
        
        if(endTimeSeconds - startTimeSeconds < 0.1) PaperLogger.debug("TODO User likely does not have the system keyboard permission");

        if(systemKeyboard->status == TouchScreenKeyboard::Status::Done) {
            inputFieldView->set_text(systemKeyboard->get_text());
            inputFieldView->get_onValueChanged()->Invoke(inputFieldView);
        }
        systemKeyboard = nullptr;
        co_return;
    }



    EntryTableCell* EntryTableCell::create() {
        PaperLogger.debug("EntryTableCell");

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

        entryTableCell->upButton = createIconButton(orderButtonsTransform, PNG_SPRITE(up), std::bind(&EntryTableCell::HandleUpButtonOnClick, entryTableCell));
        
        entryTableCell->downButton = createIconButton(orderButtonsTransform, PNG_SPRITE(down), std::bind(&EntryTableCell::HandleDownButtonOnClick, entryTableCell));
        

        GameObject* mainSectionGO = GameObject::New_ctor("MainSection");
        RectTransform* mainSectionTransform = mainSectionGO->AddComponent<RectTransform*>();
        mainSectionTransform->SetParent(entryTableCellTransform, false);
        HorizontalLayoutGroup* mainSectionHorizontal = mainSectionGO->AddComponent<HorizontalLayoutGroup*>();
        ContentSizeFitter* mainSectionFitter = mainSectionGO->AddComponent<ContentSizeFitter*>();
        mainSectionFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);

        entryTableCell->findSettingsButton = createIconButton(mainSectionTransform, PNG_SPRITE(settings), std::bind(&EntryTableCell::HandleFindSettingsButtonOnClick, entryTableCell));
        
        entryTableCell->findStringInput = BSML::Lite::CreateStringSetting(mainSectionTransform, "Find", "", std::bind(&EntryTableCell::HandleFindStringInputOnChange, entryTableCell));
        entryTableCell->findStringInput->GetComponent<LayoutElement*>()->set_preferredWidth(40);
        GameObject* findStringSystemKeyboardButtonGO = GameObject::Instantiate(entryTableCell->findStringInput->_clearSearchButton->get_gameObject(), entryTableCell->findStringInput->_clearSearchButton->get_transform()->GetParent());
        entryTableCell->findStringInput->_clearSearchButton->get_gameObject()->SetActive(false);
        findStringSystemKeyboardButtonGO->set_name("SystemKeyboardButton");
        Button* findStringSystemKeyboardButton = findStringSystemKeyboardButtonGO->GetComponent<Button*>();
        findStringSystemKeyboardButton->set_interactable(true);
        findStringSystemKeyboardButton->get_onClick()->RemoveAllListeners();
        findStringSystemKeyboardButton->get_onClick()->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(std::function<void()>([entryTableCell](){
            auto coro = custom_types::Helpers::CoroutineHelper::New(openSystemKeyboard(entryTableCell->findStringInput));
            entryTableCell->StartCoroutine(coro);
        })));
        RectTransform* findStringSystemKeyboardButtonBackgroundTransform = findStringSystemKeyboardButton->get_transform()->Find("BG")->GetComponent<RectTransform*>();
        findStringSystemKeyboardButtonBackgroundTransform->set_sizeDelta({5, 5});
        RectTransform* findStringSystemKeyboardButtonIconTransform = findStringSystemKeyboardButton->get_transform()->Find("Icon")->GetComponent<RectTransform*>();
        findStringSystemKeyboardButtonIconTransform->set_sizeDelta({5, 5});
        ImageView* findStringSystemKeyboardButtonIcon = findStringSystemKeyboardButtonIconTransform->GetComponent<ImageView*>();
        findStringSystemKeyboardButtonIcon->set_sprite(PNG_SPRITE(keyboard));

        entryTableCell->replaceStringInput = BSML::Lite::CreateStringSetting(mainSectionTransform, "Replace", "", std::bind(&EntryTableCell::HandleReplaceStringInputOnChange, entryTableCell));
        entryTableCell->replaceStringInput->GetComponent<LayoutElement*>()->set_preferredWidth(40);
        GameObject* replaceStringSystemKeyboardButtonGO = GameObject::Instantiate(entryTableCell->replaceStringInput->_clearSearchButton->get_gameObject(), entryTableCell->replaceStringInput->_clearSearchButton->get_transform()->GetParent());
        entryTableCell->replaceStringInput->_clearSearchButton->get_gameObject()->SetActive(false);
        replaceStringSystemKeyboardButtonGO->set_name("SystemKeyboardButton");
        Button* replaceStringSystemKeyboardButton = replaceStringSystemKeyboardButtonGO->GetComponent<Button*>();
        replaceStringSystemKeyboardButton->set_interactable(true);
        replaceStringSystemKeyboardButton->get_onClick()->RemoveAllListeners();
        replaceStringSystemKeyboardButton->get_onClick()->AddListener(custom_types::MakeDelegate<Events::UnityAction*>(std::function<void()>([entryTableCell](){
            auto coro = custom_types::Helpers::CoroutineHelper::New(openSystemKeyboard(entryTableCell->replaceStringInput));
            entryTableCell->StartCoroutine(coro);
        })));
        RectTransform* replaceStringSystemKeyboardButtonBackgroundTransform = replaceStringSystemKeyboardButton->get_transform()->Find("BG")->GetComponent<RectTransform*>();
        replaceStringSystemKeyboardButtonBackgroundTransform->set_sizeDelta({5, 5});
        RectTransform* replaceStringSystemKeyboardButtonIconTransform = replaceStringSystemKeyboardButton->get_transform()->Find("Icon")->GetComponent<RectTransform*>();
        replaceStringSystemKeyboardButtonIconTransform->set_sizeDelta({5, 5});
        ImageView* replaceStringSystemKeyboardButtonIcon = replaceStringSystemKeyboardButtonIconTransform->GetComponent<ImageView*>();
        replaceStringSystemKeyboardButtonIcon->set_sprite(PNG_SPRITE(keyboard));

        entryTableCell->replaceSettingsButton = createIconButton(mainSectionTransform, PNG_SPRITE(settings), std::bind(&EntryTableCell::HandleReplaceSettingsButtonOnClick, entryTableCell));

        entryTableCell->replaceSettingsButton = createIconButton(entryTableCellTransform, PNG_SPRITE(delete), std::bind(&EntryTableCell::HandleRemoveButtonOnClick, entryTableCell));

        return entryTableCell;
    }

    void EntryTableCell::updateData(FindReplaceEntry* entry) {
        PaperLogger.debug("&EntryTableCell: {}, &entry: {}", static_cast<void*>(this), static_cast<void*>(entry));
        if(!entry) {PaperLogger.error("entry is nullptr"); return;}
        
        this->entry = entry;

        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}
        std::vector<FindReplaceEntry>& entries = entryTableView->config->entries;
        int index = std::distance(entries.begin(), std::find_if(entries.begin(), entries.end(), [this](const FindReplaceEntry& x){return &x == this->entry;}));
        if(index == entries.size()) {PaperLogger.error("entry not found in entries vector"); return;}
        upButton->set_interactable(index > 0);
        downButton->set_interactable(index < entries.size() - 1);

        findStringInput->set_text(entry->getFindString());
        replaceStringInput->set_text(entry->replaceString);
    }

    void EntryTableCell::HandleUpButtonOnClick() {
        PaperLogger.debug("&EntryTableCell: {}", static_cast<void*>(this));
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}
        std::vector<FindReplaceEntry>& entries = entryTableView->config->entries;

        int index = std::distance(entries.begin(), std::find_if(entries.begin(), entries.end(), [this](const FindReplaceEntry& x){return &x == this->entry;}));
        if(index == entries.size()) {PaperLogger.error("entry not found in entries vector"); return;}
        if(index <= 0) {PaperLogger.error("Can't move entry up, entry is already at top"); return;}
        
        PaperLogger.info("Entry: '{}', Index: {} -1, Idx: {}", entry->getFindString(), index, idx);
        std::swap(entries[index], entries[index - 1]);
        MoveIdx(-1);

        entryTableView->ReloadEntryOrder();
    }

    void EntryTableCell::HandleDownButtonOnClick() {
        PaperLogger.debug("&EntryTableCell: {}", static_cast<void*>(this));
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}
        std::vector<FindReplaceEntry>& entries = entryTableView->config->entries;

        int index = std::distance(entries.begin(), std::find_if(entries.begin(), entries.end(), [this](const FindReplaceEntry& x){return &x == this->entry;}));
        if(index == entries.size()) {PaperLogger.error("entry not found in entries vector"); return;}
        if(index >= entries.size() - 1) {PaperLogger.error("Can't move entry down, entry is already at bottom"); return;}
        
        PaperLogger.info("Entry: '{}', Index: {} +1, Idx: {}", entry->getFindString(), index, idx);
        std::swap(entries[index], entries[index + 1]);
        MoveIdx(1);

        entryTableView->ReloadEntryOrder();
    }

    void EntryTableCell::HandleFindSettingsButtonOnClick() {
        PaperLogger.debug("&EntryTableCell: {}", static_cast<void*>(this));
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}

        AnyTextFlowCoordinator::GetInstance()->entryListViewController->findSettingsModal->show(entry);
    }

    void EntryTableCell::HandleFindStringInputOnChange() {
        PaperLogger.debug("&EntryTableCell: {}, findStringInput text: '{}'", static_cast<void*>(this), findStringInput->get_text());
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}

        entry->setFindString(std::string(findStringInput->get_text()));
    }

    void EntryTableCell::HandleReplaceStringInputOnChange() {
        PaperLogger.debug("&EntryTableCell: {}, replaceStringInput text: '{}'", static_cast<void*>(this), replaceStringInput->get_text());
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}

        entry->replaceString = std::string(replaceStringInput->get_text());
    }

    void EntryTableCell::HandleReplaceSettingsButtonOnClick() {
        PaperLogger.debug("&EntryTableCell: {}", static_cast<void*>(this));
    }

    void EntryTableCell::HandleRemoveButtonOnClick() {
        PaperLogger.debug("&EntryTableCell: {}", static_cast<void*>(this));
        if(!entry) {PaperLogger.error("entry is not assigned"); return;}
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}

        std::vector<FindReplaceEntry>& entries = entryTableView->config->entries;
        auto it = std::find_if(entries.begin(), entries.end(), [this](const FindReplaceEntry& x){return &x == this->entry;});
        if(it == entries.end()) {PaperLogger.error("entry not found in entries vector"); return;}
        
        PaperLogger.debug("Removing &entry: {}", static_cast<void*>(entry));
        entries.erase(it);
        entryTableView->ReloadEntryOrder();
    }

    void EntryTableCell::WasPreparedForReuse() {
        PaperLogger.debug("&EntryTableCell: {}", static_cast<void*>(this));
        entry = nullptr;
    }



    EntryCreatorTableCell* EntryCreatorTableCell::create() {
        PaperLogger.debug("EntryCreatorTableCell");

        GameObject* entryCreatorTableCellGO = GameObject::New_ctor("EntryCreatorTableCell");
        RectTransform* entryCreatorTableCellTransform = entryCreatorTableCellGO->AddComponent<RectTransform*>();
        HorizontalLayoutGroup* entryCreatorTableCellHorizontal = entryCreatorTableCellGO->AddComponent<HorizontalLayoutGroup*>();
        entryCreatorTableCellHorizontal->set_spacing(2);
        ContentSizeFitter* entryCreatorTableCellFitter = entryCreatorTableCellGO->AddComponent<ContentSizeFitter*>();
        entryCreatorTableCellFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        entryCreatorTableCellFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
        EntryCreatorTableCell* entryCreatorTableCell = entryCreatorTableCellGO->AddComponent<EntryCreatorTableCell*>();

        entryCreatorTableCell->createEntryButton = createIconButton(entryCreatorTableCellTransform, PNG_SPRITE(add), std::bind(&EntryCreatorTableCell::HandleCreateEntryButtonOnClick, entryCreatorTableCell));

        return entryCreatorTableCell;
    }

    void EntryCreatorTableCell::HandleCreateEntryButtonOnClick() {
        PaperLogger.debug("&EntryCreatorTableCell: {}", static_cast<void*>(this));
        if(!entryTableView) {PaperLogger.error("entryTableView is not valid"); return;}
        if(!entryTableView->config) {PaperLogger.error("config is not assigned"); return;}

        entryTableView->config->entries.emplace_back();
        PaperLogger.debug("Created &entry: {}", static_cast<void*>(&entryTableView->config->entries.back()));
        entryTableView->ReloadEntryOrder();
    }



    float EntryTableView::CellSize() {
        return cellSize;
    }

    int EntryTableView::NumberOfCells() {
        if(!config) {PaperLogger.error("config is not assigned"); return 0;}
        return config->entries.size() + 1; // Plus one to make room for EntryCreatorTableCell
    }

    HMUI::TableCell* EntryTableView::CellForIdx(HMUI::TableView* tableView, int idx) {
        if(!config) {PaperLogger.error("config is not assigned"); return nullptr;}
        
        this->tableView = tableView;

        if(idx < config->entries.size()) {
            UnityW<EntryTableCell> entryTableCell = static_cast<UnityW<EntryTableCell>>(tableView->DequeueReusableCellForIdentifier(entryTableCellReuseIdentifier));
            if(!entryTableCell) {
                entryTableCell = EntryTableCell::create();
                entryTableCell->entryTableView = this;
                entryTableCell->reuseIdentifier = entryTableCellReuseIdentifier;
            }
            entryTableCell->updateData(&config->entries[idx]);
            return entryTableCell;
        }
        
        UnityW<EntryCreatorTableCell> entryCreatorTableCell = static_cast<UnityW<EntryCreatorTableCell>>(tableView->DequeueReusableCellForIdentifier(entryCreatorTableCellReuseIdentifier));
        if(!entryCreatorTableCell) {
            entryCreatorTableCell = EntryCreatorTableCell::create();
            entryCreatorTableCell->entryTableView = this;
            entryCreatorTableCell->reuseIdentifier = entryCreatorTableCellReuseIdentifier;
        }
        return entryCreatorTableCell;
    }

    void EntryTableView::ReloadEntryOrder() {
        tableView->ReloadDataKeepingPosition();
    }

}