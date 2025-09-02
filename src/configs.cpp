#include "configs.hpp"
#include "main.hpp"
#include "modConfig.hpp"

#include <algorithm>
#include <filesystem>

using namespace AnyText;

namespace AnyText {

    std::vector<Config> configs;

    void loadConfigOrder() {
        std::vector<std::string> configOrder = getModConfig().configOrder.GetValue();
        PaperLogger.info("Config order: {}", configOrder);

        static auto comp = [&configOrder](const Config& a, const Config& b){
            int indexA = std::distance(configOrder.begin(), std::find(configOrder.begin(), configOrder.end(), a.name));
            int indexB = std::distance(configOrder.begin(), std::find(configOrder.begin(), configOrder.end(), b.name));
            return indexA < indexB;
        };

        std::sort(configs.begin(), configs.end(), comp);
    }

    void saveConfigOrder() {
        std::vector<std::string> configOrder;
        for(Config& config : configs) configOrder.push_back(config.name);
        getModConfig().configOrder.SetValue(configOrder);
    }
    
    void loadConfigs() {
        configs.clear();

        for(auto& file : std::filesystem::directory_iterator(getAnyTextDir())) {
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

        loadConfigOrder();
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

        saveConfigOrder();
    }

}