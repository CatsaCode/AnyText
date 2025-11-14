#include "ui/configTable.hpp"
#include "main.hpp"

#include "configs.hpp"
#include "ui/anyTextFlowCoordinator.hpp"

#include "HMUI/InputFieldView.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

#include <algorithm>
#include <functional>
#include <utility>

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, ConfigTableCell);
DEFINE_TYPE(AnyText::UI, ConfigTableView);

namespace AnyText::UI {

    ConfigTableCell* ConfigTableCell::create() {
        PaperLogger.debug("ConfigTableCell");

        GameObject* configTableCellGO = GameObject::New_ctor("ConfigTableCell");
        RectTransform* configTableCellTransform = configTableCellGO->AddComponent<RectTransform*>();
        HorizontalLayoutGroup* configTableCellHorizontal = configTableCellGO->AddComponent<HorizontalLayoutGroup*>();
        configTableCellHorizontal->set_spacing(2);
        ContentSizeFitter* configTableCellFitter = configTableCellGO->AddComponent<ContentSizeFitter*>();
        configTableCellFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        configTableCellFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
        ConfigTableCell* configTableCell = configTableCellGO->AddComponent<ConfigTableCell*>();

        GameObject* orderButtonsGO = GameObject::New_ctor("OrderButtons");
        RectTransform* orderButtonsTransform = orderButtonsGO->AddComponent<RectTransform*>();
        orderButtonsTransform->SetParent(configTableCellTransform, false);
        HorizontalLayoutGroup* orderButtonsHorizontal = orderButtonsGO->AddComponent<HorizontalLayoutGroup*>();
        ContentSizeFitter* orderButtonsFitter = orderButtonsGO->AddComponent<ContentSizeFitter*>();
        orderButtonsFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);

        configTableCell->upButton = BSML::Lite::CreateUIButton(orderButtonsTransform, "▲", std::bind(&ConfigTableCell::HandleMoveUpOnClick, configTableCell));
        configTableCell->upButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);
        
        configTableCell->downButton = BSML::Lite::CreateUIButton(orderButtonsTransform, "▼", std::bind(&ConfigTableCell::HandleMoveDownOnClick, configTableCell));
        configTableCell->downButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);
        
        configTableCell->nameInput = BSML::Lite::CreateStringSetting(configTableCellTransform, "Config name", "", std::bind(&ConfigTableCell::HandleNameInputOnChange, configTableCell));
        configTableCell->nameInput->_clearSearchButton->get_gameObject()->SetActive(false);
        configTableCell->nameInput->GetComponent<LayoutElement*>()->set_preferredWidth(70);
        
        configTableCell->editButton = BSML::Lite::CreateUIButton(configTableCellTransform, "Edit", std::bind(&ConfigTableCell::HandleEditButtonOnClick, configTableCell));
        configTableCell->editButton->GetComponent<LayoutElement*>()->set_preferredWidth(12);

        configTableCell->removeButton = BSML::Lite::CreateUIButton(configTableCellTransform, "X", std::bind(&ConfigTableCell::HandleRemoveButtonOnClick, configTableCell));
        configTableCell->removeButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);

        return configTableCell;
    }
    
    void ConfigTableCell::updateData(Config* config) {
        PaperLogger.debug("&ConfigTableCell: {}, &config: {}", static_cast<void*>(this), static_cast<void*>(config));
        if(!config) {PaperLogger.error("config is nullptr"); return;}
        this->config = config;

        int index = std::distance(configs.begin(), std::find_if(configs.begin(), configs.end(), [this](const Config& x){return &x == this->config;}));
        if(index == configs.size()) {PaperLogger.error("config not found in configs vector"); return;}
        upButton->set_interactable(index > 0);
        downButton->set_interactable(index < configs.size() - 1);

        nameInput->set_text(config->name);
    }

    void ConfigTableCell::HandleMoveUpOnClick() {
        PaperLogger.debug("&ConfigTableCell: {}", static_cast<void*>(this));
        if(!config) {PaperLogger.error("Config is not assigned"); return;}
        if(!configTableView) {PaperLogger.error("configTableView is not valid"); return;}

        int index = std::distance(configs.begin(), std::find_if(configs.begin(), configs.end(), [this](const Config& x){return &x == this->config;}));
        if(index == configs.size()) {PaperLogger.error("Config not found in configs vector"); return;}
        if(index <= 0) {PaperLogger.error("Can't move config up, config is already at top"); return;};
        
        PaperLogger.info("Config: '{}', Index: {} -1, Idx: {}", config->name, index, idx);
        std::swap(configs[index], configs[index - 1]);
        MoveIdx(-1);

        configTableView->ReloadConfigOrder();
    }

    void ConfigTableCell::HandleMoveDownOnClick() {
        PaperLogger.debug("&ConfigTableCell: {}", static_cast<void*>(this));
        if(!config) {PaperLogger.error("Config is not assigned"); return;}
        if(!configTableView) {PaperLogger.error("configTableView is not valid"); return;}

        int index = std::distance(configs.begin(), std::find_if(configs.begin(), configs.end(), [this](const Config& x){return &x == this->config;}));
        if(index == configs.size()) {PaperLogger.error("Config not found in configs vector"); return;}
        if(index >= configs.size() - 1) {PaperLogger.error("Can't move config down, config is already at bottom"); return;};
        
        PaperLogger.info("Config: '{}', Index: {} +1, Idx: {}", config->name, index, idx);
        std::swap(configs[index], configs[index + 1]);
        MoveIdx(1);

        configTableView->ReloadConfigOrder();
    }

    void ConfigTableCell::HandleNameInputOnChange() {
        PaperLogger.debug("&ConfigTableCell: {}, nameInput text: '{}'", static_cast<void*>(this), nameInput->get_text());
        if(!config) {PaperLogger.error("Config is not assigned"); return;}

        config->name = std::string(nameInput->get_text());
        config->unsaved = true;
    }

    void ConfigTableCell::HandleEditButtonOnClick() {
        PaperLogger.debug("&ConfigTableCell: {}", static_cast<void*>(this));
        if(!config) {PaperLogger.error("Config is not assigned"); return;}
        
        config->unsaved = true;
        AnyTextFlowCoordinator::GetInstance()->presentEntries(config);
    }

    void ConfigTableCell::HandleRemoveButtonOnClick() {
        PaperLogger.debug("&ConfigTableCell: {}", static_cast<void*>(this));
        if(!config) {PaperLogger.error("Config is not assigned"); return;}
        if(!configTableView) {PaperLogger.error("configTableView is not assigned"); return;}

        removeConfig(*config);
        configTableView->ReloadConfigOrder();
    }

    void ConfigTableCell::WasPreparedForReuse() {
        PaperLogger.debug("&ConfigTableCell: {}", static_cast<void*>(this));
        config = nullptr;
    }



    void ConfigTableView::ctor() {
        INVOKE_CTOR();
    }
    
    float ConfigTableView::CellSize() {
        return cellSize;
    }

    int ConfigTableView::NumberOfCells() {
        return configs.size();
    }

    HMUI::TableCell* ConfigTableView::CellForIdx(HMUI::TableView* tableView, int idx) {
        this->tableView = tableView;
        UnityW<ConfigTableCell> configTableCell = static_cast<UnityW<ConfigTableCell>>(tableView->DequeueReusableCellForIdentifier(reuseIdentifier));
        if(!configTableCell) {
            configTableCell = ConfigTableCell::create();
            configTableCell->configTableView = this;
            configTableCell->reuseIdentifier = reuseIdentifier;
        }
        configTableCell->updateData(&configs[idx]);
        return configTableCell;
    }

    void ConfigTableView::ReloadConfigOrder() {
        tableView->ReloadDataKeepingPosition();
    }

}