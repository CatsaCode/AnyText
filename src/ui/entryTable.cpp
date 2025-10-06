#include "ui/entryTable.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

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

        entryTableCell->findStringInput = BSML::Lite::CreateStringSetting(entryTableCellTransform, "Find", "", nullptr);
        entryTableCell->findStringInput->_clearSearchButton->get_gameObject()->SetActive(false);
        entryTableCell->findStringInput->GetComponent<LayoutElement*>()->set_preferredWidth(70);

        return entryTableCell;
    }

    void EntryTableCell::updateData(FindReplaceEntry* entry) {
        PaperLogger.info(__func__);

        this->entry = entry;

        findStringInput->set_text(entry->findString);
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