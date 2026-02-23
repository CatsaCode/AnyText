#pragma once

#include "findReplaceEntry.hpp"

#include "config-utils/shared/config-utils.hpp"

#include <filesystem>

namespace AnyText {

    DECLARE_JSON_STRUCT(Config) {
        VALUE_DEFAULT(int, version, 0);
        VALUE_DEFAULT(std::vector<FindReplaceEntry>, entries, {});

        std::filesystem::path filePath;
        std::string name;

        bool unsaved = false;
        bool enabled = true;
    };

    extern std::vector<Config> configs;
    void loadConfigOrder();
    void saveConfigOrder();
    void loadConfigs();
    bool saveConfig(Config& config);
    void saveConfigs();
    void removeConfig(const Config& config);
}