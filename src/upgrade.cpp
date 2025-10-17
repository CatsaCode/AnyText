#include "modConfig.hpp"
#include "main.hpp"

#include "findReplaceEntry.hpp"
#include "configs.hpp"

#include <algorithm>

namespace AnyText {

    void ModConfig_t::upgradeVersion(rapidjson::Value& jsonValue) {
        if(!jsonValue.HasMember("findStrings") || !jsonValue.HasMember("replaceStrings")) return;
        rapidjson::Value& findStrings = jsonValue["findStrings"];
        rapidjson::Value& replaceStrings = jsonValue["replaceStrings"];
        if(!findStrings.IsArray() || !replaceStrings.IsArray()) return;
    
        PaperLogger.info("Attempting upgrade from v1.0.1 to v1.1.0 format");
        Config& upgradedConfig = configs.emplace_back();
        upgradedConfig.name = "FAILED_TO_UPGRADE_CONFIG";

        if(findStrings.GetArray().Size() != replaceStrings.GetArray().Size()) PaperLogger.warn("findStrings and replaceStrings have a different size: {}-{}", findStrings.GetArray().Size(), replaceStrings.GetArray().Size());
        int numEntries = std::min(findStrings.GetArray().Size(), replaceStrings.GetArray().Size());
        PaperLogger.info("numEntries: {}", numEntries);

        for(int i = 0; i < numEntries; i++) {
            if(!findStrings.GetArray()[i].IsString() || !replaceStrings.GetArray()[i].IsString()) {PaperLogger.error("Unexpected data type"); return;}
            FindReplaceEntry& entry = upgradedConfig.entries.emplace_back();
            entry.setFindString(findStrings.GetArray()[i].GetString());
            entry.setFindAlgorithm(FindAlgorithm::ExactMatch);
            entry.replaceString = replaceStrings.GetArray()[i].GetString();
        }

        upgradedConfig.name = "UpgradedConfig";
        upgradedConfig.unsaved = true;
        if(!saveConfig(upgradedConfig)) return;
        jsonValue.RemoveMember("findStrings");
        jsonValue.RemoveMember("replaceStrings");
    }


}
