#include "configs.hpp"
#include "main.hpp"
#include "modConfig.hpp"

#include <algorithm>
#include <filesystem>

using namespace AnyText;

namespace AnyText {

    std::vector<Config> configs;


    void FindReplaceEntry::updateFindRegex() {
        std::string findRegexStr = findString;
        if(getFindAlgorithm() != FindAlgorithm::Regex) {
            static const std::regex escapeRegex ("[\\+\\*\\?\\^\\$\\\\\\.\\[\\]\\{\\}\\(\\)\\|\\/]");
            findRegexStr = std::regex_replace(findRegexStr, escapeRegex, "\\$&");
        }
        if(getFindAlgorithm() == FindAlgorithm::ExactMatch)
            findRegexStr = '^' + findRegexStr + "$";
        findRegex = findRegexStr;

        PaperLogger.info("Converting findString: '{}' to regex: '{}'", findString, findRegexStr);
    }


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

            PaperLogger.info("Loading config from path: '{}'", file.path().string());
            Config config;
            try {
                ReadFromFile(file.path().string(), config);
            } catch(const std::exception& err) {
                PaperLogger.error("Failed to load config from path: '{}', Error: {}", file.path().string(), err.what());
                continue;
            }

            config.filePath = file.path();
            config.name = file.path().stem(); // TODO Sanitize

            configs.push_back(config);
        }

        loadConfigOrder();
    }

    bool saveConfig(Config& config) {
        if(!config.unsaved) return false;

        std::filesystem::path filePath (std::string(getAnyTextDir()) + config.name + ".json"); // TODO Sanitize
        PaperLogger.info("Saving config '{}' to path: '{}'", config.name, filePath.string());
        
        try {
            WriteToFile(filePath.string(), config, true);
        } catch(const std::exception& err) {
            PaperLogger.error("Failed to save config: '{}', Path: '{}', Error: '{}'", config.name, filePath.string(), err.what());
            return false;
        }

        config.unsaved = false;
        if(config.filePath.empty()) config.filePath = filePath;

        if(config.filePath.stem().string() == config.name) return true;
        PaperLogger.info("Removing config '{}' from previous path: '{}'", config.name, config.filePath.string());
        if(!std::filesystem::equivalent(config.filePath.parent_path(), getAnyTextDir())) {PaperLogger.error("Config not in AnyText directory"); return true;}
        std::filesystem::remove(config.filePath);
        config.filePath = filePath;
        
        return true;
    }

    void saveConfigs() {
        for(Config& config : configs) {
            saveConfig(config);
        }

        saveConfigOrder();
    }

    void removeConfig(const Config& config) {
        PaperLogger.info("Removing config '{}' at path: '{}'", config.name, config.filePath.string());
        
        if(!std::filesystem::equivalent(config.filePath.parent_path(), getAnyTextDir())) {PaperLogger.error("Config not in AnyText directory"); return;}
        std::filesystem::remove(config.filePath);

        auto it = std::find_if(configs.begin(), configs.end(), [&config](const Config& x){return &x == &config;});
        if(it != configs.end()) configs.erase(it);
    }

}