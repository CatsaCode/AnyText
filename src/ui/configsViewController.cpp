#include "ui/configsViewController.hpp"
#include "main.hpp"

#include "configs.hpp"
#include "ui/configTable.hpp"

#include "bsml/shared/BSML.hpp"

#include "custom-types/shared/delegate.hpp"

#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;

DEFINE_TYPE(AnyText::UI, ConfigsViewController);

namespace AnyText::UI {

    static UnityW<VerticalLayoutGroup> configsVertical;

    void ConfigsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        if(!firstActivation) return;

        configTableView = BSML::Lite::CreateScrollableCustomSourceList<ConfigTableView*>(transform);
        if(configTableView->tableView) configTableView->tableView->ReloadData();

        // BSML::Lite::CreateScrollView(transform);
    }

}