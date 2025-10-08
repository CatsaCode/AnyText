#pragma once

#include "configs.hpp"

#include "config-utils/shared/config-utils.hpp"

#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/InputFieldView.hpp"

#include "UnityEngine/UI/Button.hpp"

DECLARE_CLASS_CODEGEN(AnyText::UI, EntryTableCell, HMUI::TableCell) {
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, upButton);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, downButton);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, findSettingsButton);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::InputFieldView>, findStringInput);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::InputFieldView>, replaceStringInput);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, replaceSettingsButton);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, removeButton);

    DECLARE_INSTANCE_METHOD(void, HandleUpButtonOnClick);
    DECLARE_INSTANCE_METHOD(void, HandleDownButtonOnClick);
    DECLARE_INSTANCE_METHOD(void, HandleFindSettingsButtonOnClick);
    DECLARE_INSTANCE_METHOD(void, HandleFindStringInputOnChange);
    DECLARE_INSTANCE_METHOD(void, HandleReplaceStringInputOnChange);
    DECLARE_INSTANCE_METHOD(void, HandleReplaceSettingsButtonOnClick);
    DECLARE_INSTANCE_METHOD(void, HandleRemoveButtonOnClick);

    DECLARE_OVERRIDE_METHOD_MATCH(void, WasPreparedForReuse, &HMUI::TableCell::WasPreparedForReuse);

    public:
        FindReplaceEntry* entry;

        void updateData(FindReplaceEntry* entry);

        static EntryTableCell* create();
};

DECLARE_CLASS_CODEGEN_INTERFACES(AnyText::UI, EntryTableView, UnityEngine::MonoBehaviour, HMUI::TableView::IDataSource*) {
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::TableView>, tableView);
    DECLARE_INSTANCE_FIELD_DEFAULT(float, cellSize, 8);
    DECLARE_INSTANCE_FIELD_DEFAULT(StringW, reuseIdentifier, "AnyTextEntryTableCell");

    DECLARE_DEFAULT_CTOR();

    DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);
    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);

    public:
        Config* config;
};