#include "configs.hpp"
#include "main.hpp"

#include <filesystem>

using namespace AnyText;

namespace AnyText {

    std::vector<Config> configs;
    
    void loadConfigs() {
        configs.clear();

        for(auto& file : std::filesystem::recursive_directory_iterator(getAnyTextDir())) {
            if(file.path().extension() != ".json") continue;

            std::string filePath = file.path();
            PaperLogger.info("Loading config from path: '{}'", filePath);
            Config config;
            try {
                ReadFromFile(filePath, config);
            } catch(const std::exception& err) {
                PaperLogger.error("Failed to load config '{}', Error: {}", filePath, err.what());
                continue;
            }

            configs.push_back(config);

            // DEBUG
            PaperLogger.info("Version: {}", config.version);
            for(FindReplaceEntry& entry : config.entries) {
                PaperLogger.info("Find: '{}', Replace: '{}'", entry.findString, entry.replaceString);
            }
        }
    }

}