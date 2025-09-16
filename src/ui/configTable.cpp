#include "ui/configTable.hpp"
#include "main.hpp"

#include "configs.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include <functional>

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, ConfigTableCell);
DEFINE_TYPE(AnyText::UI, ConfigTableView);

namespace AnyText::UI {

    ConfigTableCell* ConfigTableCell::create() {
        PaperLogger.info("Creating ConfigTableCell...");

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

        configTableCell->upButton = BSML::Lite::CreateUIButton(orderButtonsTransform, "Up");
        configTableCell->upButton->GetComponent<LayoutElement*>()->set_preferredWidth(10);
        
        configTableCell->downButton = BSML::Lite::CreateUIButton(orderButtonsTransform, "Down");
        configTableCell->downButton->GetComponent<LayoutElement*>()->set_preferredWidth(10);
        
        configTableCell->nameInput = BSML::Lite::CreateStringSetting(configTableCellTransform, "Config name", "", std::bind(&ConfigTableCell::HandleNameInputOnChange, configTableCell));
        configTableCell->nameInput->GetComponent<LayoutElement*>()->set_preferredWidth(70);
        
        configTableCell->editButton = BSML::Lite::CreateUIButton(configTableCellTransform, "Edit", std::bind(&ConfigTableCell::HandleEditButtonOnClick, configTableCell));
        configTableCell->editButton->GetComponent<LayoutElement*>()->set_preferredWidth(12);

        configTableCell->deleteButton = BSML::Lite::CreateUIButton(configTableCellTransform, "X");
        configTableCell->deleteButton->GetComponent<LayoutElement*>()->set_preferredWidth(6);

        return configTableCell;
    }
    
    void ConfigTableCell::HandleNameInputOnChange() {
        PaperLogger.info("HandleNameInputOnChange");
    }

    void ConfigTableCell::HandleEditButtonOnClick() {
        PaperLogger.info("HandleEditButtonOnClick");
    }

    void ConfigTableCell::updateData(Config* config) {
        PaperLogger.info("UpdateData with config: '{}'", config->name);

        this->config = config;

        nameInput->set_text(config->name);
    }

    void ConfigTableCell::WasPreparedForReuse() {
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
            configTableCell->reuseIdentifier = reuseIdentifier;
        }
        configTableCell->updateData(&configs[idx]);
        return configTableCell;
    }

}