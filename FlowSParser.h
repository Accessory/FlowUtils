#pragma once

#include <string>



namespace FlowSParser {
    using namespace std;

    inline bool startWith(const std::string& value, const std::string& toCheck) {
        return value.substr(0, toCheck.length()) == toCheck;
    }

    inline size_t nextSizeT(const std::string& value, size_t* pos){
        return std::stoull(value, pos, 10);
    }

    inline std::string goTo(const std::string &text, const std::string &toGoTo, size_t &pos) {
        size_t start = pos;
        pos = text.find(toGoTo, pos);
        return text.substr(start, pos - start);
    }

    inline std::string gotoNextNonWhite(const std::string &text, size_t &pos) {
        size_t start = pos;
        pos = text.find_first_not_of(" \n\r", pos);
        return text.substr(start, pos - start);
    }
    inline std::string gotoNextNonAlpha(const std::string &text, size_t &pos) {
        size_t start = pos;
        pos = text.find_first_of(" \n\r", pos);
        return text.substr(start, pos - start);
    }

    inline std::string goToNewLine(const std::string &text, size_t &pos) {
        size_t start = pos;
        pos = text.find_first_of("\n\r", pos);
        return text.substr(start, pos - start);
    }

};
