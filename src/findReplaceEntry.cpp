#include "findReplaceEntry.hpp"
#include "main.hpp"

#include "boost/regex.hpp"

namespace AnyText {

    void FindReplaceEntry::updateFindRegex() {
        std::string findRegexStr = findString;

        if(getFindAlgorithm() != FindAlgorithm::Regex) {
            static const boost::regex escapeRegex ("[\\+\\*\\?\\^\\$\\\\\\.\\[\\]\\{\\}\\(\\)\\|\\/]");
            findRegexStr = boost::regex_replace(findRegexStr, escapeRegex, "\\\\$&");
        }

        if(getFindAlgorithm() == FindAlgorithm::ExactMatch)
            findRegexStr = '^' + findRegexStr + "$";

        boost::regex::flag_type flags = boost::regex::ECMAScript | boost::regex::optimize;
        if(!getMatchCase()) flags |= boost::regex::icase;


        PaperLogger.info("Converting findString: '{}' to regex: '{}', flags: {}", findString, findRegexStr, static_cast<int>(flags));
        
        try {
            findRegex = boost::regex(findRegexStr, flags);
            findRegexIsValid = true;
        } catch(const boost::regex_error e) {
            PaperLogger.info("Invalid regex, code: {}", static_cast<int>(e.code()));
            findRegex = boost::regex("");
            findRegexIsValid = false;
        }

        if(findRegexStr.empty()) findRegexIsValid = false;
    }

}