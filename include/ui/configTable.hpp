#pragma once

#include "configs.hpp"

#include "custom-types/shared/macros.hpp"

#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/InputFieldView.hpp"

#include "UnityEngine/UI/Button.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, ConfigTableCell, HMUI::TableCell) {
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, upButton);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, downButton);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::InputFieldView>, nameInput);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, editButton);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, deleteButton);
    
    DECLARE_INSTANCE_METHOD(void, HandleNameInputOnChange);
    DECLARE_INSTANCE_METHOD(void, HandleEditButtonOnClick);

    DECLARE_OVERRIDE_METHOD_MATCH(void, WasPreparedForReuse, &HMUI::TableCell::WasPreparedForReuse);

    public:
        Config* config;

        void updateData(Config* config);
        
        static ConfigTableCell* create();
};

DECLARE_CLASS_CODEGEN_INTERFACES(AnyText::UI, ConfigTableView, UnityEngine::MonoBehaviour, HMUI::TableView::IDataSource*) {
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::TableView>, tableView);
    DECLARE_INSTANCE_FIELD_DEFAULT(float, cellSize, 8);
    DECLARE_INSTANCE_FIELD_DEFAULT(StringW, reuseIdentifier, "AnyTextConfigTableCell");
    
    DECLARE_CTOR(ctor);

    DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);
    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
};