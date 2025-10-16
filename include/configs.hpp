#pragma once

#include "config-utils/shared/config-utils.hpp"

#include "boost-regex/regex/include/boost/regex.hpp"

#include <filesystem>

namespace AnyText {

    enum class FindAlgorithm {
        ExactMatch,
        PartialMatch,
        Regex
    };
    static std::array<std::string_view, 3> FindAlgorithm_Strings = {"Exact Match", "Partial Match", "Regex"}; // Thanks BSML for not letting me safely make this const

    // enum class FindLocation {
    //     MenuScene,
    //     GameScene
    // };

    DECLARE_JSON_STRUCT(FindReplaceEntry) {
        private:
            VALUE(std::string, findString);
            VALUE_DEFAULT(int, findAlgorithm, static_cast<int>(FindAlgorithm::PartialMatch));
            // VALUE_DEFAULT(bool, matchCase, false);
            // VALUE_OPTIONAL(FindLocation, findLocation);

        public:
            VALUE(std::string, replaceString);
            VALUE_DEFAULT(bool, accumulate, false);

            // VALUE_OPTIONAL(bool, replaceAll);
            // VALUE_OPTIONAL(bool, bold);
            // VALUE_OPTIONAL(bool, italics);
            // VALUE_OPTIONAL(bool, underline);
            // VALUE_OPTIONAL(bool, strikethrough);
            // VALUE_OPTIONAL(bool, lowercase);
            // VALUE_OPTIONAL(bool, uppercase);
            // VALUE_OPTIONAL(bool, smallcaps);
            // VALUE_OPTIONAL(int, fontSize);
            // VALUE_OPTIONAL(bool, autoSize);
            // Color
            // Gradient
            // Spacing options
            // Alignment
            // Wrapping
            // Overflow

            
        private:
            boost::regex findRegex;

            void updateFindRegex();

            DESERIALIZE_FUNCTION(initFromJSON) {updateFindRegex();}

        public:
            void setFindString(std::string_view value) {findString = value; updateFindRegex();}
            std::string_view getFindString() const {return findString;}
            const boost::regex& getFindRegex() const {return findRegex;}

            void setFindAlgorithm(FindAlgorithm value) {findAlgorithm = std::clamp<int>(static_cast<int>(value), 0, FindAlgorithm_Strings.size() - 1); updateFindRegex();}
            FindAlgorithm getFindAlgorithm() const {return static_cast<FindAlgorithm>(findAlgorithm);}
    };

    DECLARE_JSON_STRUCT(Config) {
        VALUE_DEFAULT(int, version, 0);
        VALUE_DEFAULT(std::vector<FindReplaceEntry>, entries, {});

        std::filesystem::path filePath;
        std::string name;

        bool unsaved = false;
    };

    extern std::vector<Config> configs;
    void loadConfigOrder();
    void saveConfigOrder();
    void loadConfigs();
    bool saveConfig(Config& config);
    void saveConfigs();
    void removeConfig(const Config& config);
}