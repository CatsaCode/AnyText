#pragma once

#include "main.hpp"

#include "config-utils/shared/config-utils.hpp"

#include "boost/regex.hpp"

namespace AnyText {

    enum class FindAlgorithm {
        ExactMatch,
        PartialMatch,
        Regex
    };
    static std::array<std::string_view, 3> FindAlgorithm_Strings = {"Exact Match", "Partial Match", "RegEx"}; // Thanks BSML for not letting me safely make this const

    // enum class FindLocation {
    //     MenuScene,
    //     GameScene
    // };

    DECLARE_JSON_STRUCT(FindReplaceEntry) {
        private:
            VALUE(std::string, findString);
            VALUE_DEFAULT(int, findAlgorithm, static_cast<int>(FindAlgorithm::PartialMatch));
            VALUE_DEFAULT(bool, matchCase, false);
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

            void updateFindRegex() {
                std::string findRegexStr = findString;
                if(getFindAlgorithm() != FindAlgorithm::Regex) {
                    static const boost::regex escapeRegex ("[\\+\\*\\?\\^\\$\\\\\\.\\[\\]\\{\\}\\(\\)\\|\\/]");
                    findRegexStr = boost::regex_replace(findRegexStr, escapeRegex, "\\\\$&");
                }
                if(getFindAlgorithm() == FindAlgorithm::ExactMatch)
                    findRegexStr = '^' + findRegexStr + "$";

                boost::regex::flag_type flags = boost::regex::ECMAScript | boost::regex::optimize;
                if(!getMatchCase()) flags |= boost::regex::icase;

                findRegex = boost::regex(findRegexStr, flags);

                PaperLogger.info("Converting findString: '{}' to regex: '{}', flags: {}", findString, findRegexStr, static_cast<int>(flags));
            }

            DESERIALIZE_FUNCTION(initFromJSON) {updateFindRegex();}

        public:
            void setFindString(std::string_view value) {findString = value; updateFindRegex();}
            std::string_view getFindString() const {return findString;}
            
            void setFindAlgorithm(FindAlgorithm value) {findAlgorithm = std::clamp<int>(static_cast<int>(value), 0, FindAlgorithm_Strings.size() - 1); updateFindRegex();}
            FindAlgorithm getFindAlgorithm() const {return static_cast<FindAlgorithm>(findAlgorithm);}
            
            void setMatchCase(bool value) {matchCase = value; updateFindRegex();}
            bool getMatchCase() const {return matchCase;}

            const boost::regex& getFindRegex() const {return findRegex;}
    };

}