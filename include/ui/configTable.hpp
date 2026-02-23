#pragma once

#include "configs.hpp"

#include "custom-types/shared/macros.hpp"

#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"

#include "HMUI/TableCell.hpp"
#include "HMUI/TableView.hpp"
#include "HMUI/InputFieldView.hpp"

#include "UnityEngine/UI/Button.hpp"

namespace AnyText::UI {class ConfigTableView;}

DECLARE_CLASS_CODEGEN(AnyText::UI, ConfigTableCell, HMUI::TableCell) {
    DECLARE_INSTANCE_FIELD(UnityW<ConfigTableView>, configTableView);

    DECLARE_INSTANCE_FIELD(UnityW<BSML::ToggleSetting>, enabledToggle);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, upButton);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, downButton);
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::InputFieldView>, nameInput);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, editButton);
    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, removeButton);
    
    DECLARE_INSTANCE_METHOD(void, HandleEnabledOnChange, bool value);
    DECLARE_INSTANCE_METHOD(void, HandleMoveDownOnClick);
    DECLARE_INSTANCE_METHOD(void, HandleMoveUpOnClick);
    DECLARE_INSTANCE_METHOD(void, HandleNameInputOnChange, StringW value);
    DECLARE_INSTANCE_METHOD(void, HandleEditButtonOnClick);
    DECLARE_INSTANCE_METHOD(void, HandleRemoveButtonOnClick);

    DECLARE_OVERRIDE_METHOD_MATCH(void, WasPreparedForReuse, &HMUI::TableCell::WasPreparedForReuse);

    public:
        Config* config;

        void updateData(Config* config);
        
        static ConfigTableCell* create();
};

DECLARE_CLASS_CODEGEN(AnyText::UI, ConfigCreatorTableCell, HMUI::TableCell) {
    DECLARE_INSTANCE_FIELD(UnityW<ConfigTableView>, configTableView);

    DECLARE_INSTANCE_FIELD(UnityW<UnityEngine::UI::Button>, createConfigButton);

    DECLARE_INSTANCE_METHOD(void, HandleCreateConfigButtonOnClick);

    public:
        static ConfigCreatorTableCell* create();
};

DECLARE_CLASS_CODEGEN_INTERFACES(AnyText::UI, ConfigTableView, UnityEngine::MonoBehaviour, HMUI::TableView::IDataSource*) {
    DECLARE_INSTANCE_FIELD(UnityW<HMUI::TableView>, tableView);
    DECLARE_INSTANCE_FIELD_DEFAULT(float, cellSize, 8);
    DECLARE_INSTANCE_FIELD_DEFAULT(StringW, configTableCellReuseIdentifier, "AnyTextConfigTableCell");
    DECLARE_INSTANCE_FIELD_DEFAULT(StringW, configCreatorTableCellReuseIdentifier, "AnyTextConfigCreatorTableCell");
    
    DECLARE_CTOR(ctor);

    DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
    DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);
    DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);

    DECLARE_INSTANCE_METHOD(void, ReloadConfigOrder);
};