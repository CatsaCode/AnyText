#include "ui/configTable.hpp"
#include "main.hpp"
#include "assets.hpp"

#include "configs.hpp"
#include "ui/anyTextFlowCoordinator.hpp"
#include "ui/utils.hpp"

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
DEFINE_TYPE(AnyText::UI, ConfigCreatorTableCell);
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

        configTableCell->upButton = createIconButton(orderButtonsTransform, PNG_SPRITE(up), std::bind(&ConfigTableCell::HandleMoveUpOnClick, configTableCell));
        configTableCell->downButton = createIconButton(orderButtonsTransform, PNG_SPRITE(down), std::bind(&ConfigTableCell::HandleMoveDownOnClick, configTableCell));
        
        configTableCell->nameInput = BSML::Lite::CreateStringSetting(configTableCellTransform, "Config name", "", std::bind(&ConfigTableCell::HandleNameInputOnChange, configTableCell));
        configTableCell->nameInput->_clearSearchButton->get_gameObject()->SetActive(false);
        configTableCell->nameInput->GetComponent<LayoutElement*>()->set_preferredWidth(70);
        
        configTableCell->editButton = createIconButton(configTableCellTransform, PNG_SPRITE(edit), std::bind(&ConfigTableCell::HandleEditButtonOnClick, configTableCell));

        configTableCell->removeButton = createIconButton(configTableCellTransform, PNG_SPRITE(delete), std::bind(&ConfigTableCell::HandleRemoveButtonOnClick, configTableCell));

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



    ConfigCreatorTableCell* ConfigCreatorTableCell::create() {
        PaperLogger.debug("ConfigCreatorTableCell");

        GameObject* configCreatorTableCellGO = GameObject::New_ctor("ConfigCreatorTableCell");
        RectTransform* configCreatorTableCellTransform = configCreatorTableCellGO->AddComponent<RectTransform*>();
        HorizontalLayoutGroup* configCreatorTableCellHorizontal = configCreatorTableCellGO->AddComponent<HorizontalLayoutGroup*>();
        configCreatorTableCellHorizontal->set_spacing(2);
        ContentSizeFitter* configCreatorTableCellFitter = configCreatorTableCellGO->AddComponent<ContentSizeFitter*>();
        configCreatorTableCellFitter->set_horizontalFit(ContentSizeFitter::FitMode::PreferredSize);
        configCreatorTableCellFitter->set_verticalFit(ContentSizeFitter::FitMode::PreferredSize);
        ConfigCreatorTableCell* configCreatorTableCell = configCreatorTableCellGO->AddComponent<ConfigCreatorTableCell*>();

        configCreatorTableCell->createConfigButton = createIconButton(configCreatorTableCellTransform, PNG_SPRITE(add), std::bind(&ConfigCreatorTableCell::HandleCreateConfigButtonOnClick, configCreatorTableCell));

        return configCreatorTableCell;
    }

    void ConfigCreatorTableCell::HandleCreateConfigButtonOnClick() {
        PaperLogger.debug("&ConfigCreatorTableCell: {}", static_cast<void*>(this));
        if(!configTableView) {PaperLogger.error("configTableView is not valid"); return;}

        configs.emplace_back();
        PaperLogger.debug("Created &config: {}", static_cast<void*>(&configs.back()));
        configTableView->ReloadConfigOrder();
    }



    void ConfigTableView::ctor() {
        INVOKE_CTOR();
    }
    
    float ConfigTableView::CellSize() {
        return cellSize;
    }

    int ConfigTableView::NumberOfCells() {
        return configs.size() + 1; // Plus one to make room for ConfigCreatorTableCell
    }

    HMUI::TableCell* ConfigTableView::CellForIdx(HMUI::TableView* tableView, int idx) {
        this->tableView = tableView;
        
        if(idx < configs.size()) {
            UnityW<ConfigTableCell> configTableCell = static_cast<UnityW<ConfigTableCell>>(tableView->DequeueReusableCellForIdentifier(configTableCellReuseIdentifier));
            if(!configTableCell) {
                configTableCell = ConfigTableCell::create();
                configTableCell->configTableView = this;
                configTableCell->reuseIdentifier = configTableCellReuseIdentifier;
            }
            configTableCell->updateData(&configs[idx]);
            return configTableCell;
        }

        UnityW<ConfigCreatorTableCell> configCreatorTableCell = static_cast<UnityW<ConfigCreatorTableCell>>(tableView->DequeueReusableCellForIdentifier(configCreatorTableCellReuseIdentifier));
        if(!configCreatorTableCell) {
            configCreatorTableCell = ConfigCreatorTableCell::create();
            configCreatorTableCell->configTableView = this;
            configCreatorTableCell->reuseIdentifier = configCreatorTableCellReuseIdentifier;
        }
        return configCreatorTableCell;
    }

    void ConfigTableView::ReloadConfigOrder() {
        tableView->ReloadDataKeepingPosition();
    }

}