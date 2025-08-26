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
                PaperLogger.error("Failed to load config from path: '{}', Error: {}", filePath, err.what());
                continue;
            }

            config.name = file.path().stem(); // TODO Sanitize

            configs.push_back(config);
        }
    }

    void saveConfig(Config& config) {
        std::string filePath = std::string(getAnyTextDir()) + config.name + ".json"; // TODO Sanitize
        PaperLogger.info("Saving config to path: '{}'", filePath);
        try {
            WriteToFile(filePath, config, true);
        } catch(const std::exception& err) {
            PaperLogger.error("Failed to save config: '{}', Path: '{}', Error: '{}'", config.name, filePath, err.what());
        }
    }

    void saveConfigs() {
        for(Config& config : configs) {
            saveConfig(config);
        }
    }

}