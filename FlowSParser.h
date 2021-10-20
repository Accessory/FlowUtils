#pragma once

#include <string>


namespace FlowSParser {
    inline const std::string NON_WHITE = " \n\r";

    inline bool startWith(const std::string &value, const std::string &toCheck) {
        return value.substr(0, toCheck.length()) == toCheck;
    }

    inline size_t nextSizeT(const std::string &value, size_t *pos) {
        return std::stoull(value, pos, 10);
    }

    inline std::string goTo(const std::string &text, const std::string &toGoTo, size_t &pos) {
        const size_t start = pos;
        pos = text.find(toGoTo, pos);
        return text.substr(start, pos - start);
    }

    inline std::string nextNonWhite(const std::string &text, size_t &pos) {
        const size_t start = pos;
        pos = text.find_first_not_of(NON_WHITE, pos);
        return text.substr(start, pos - start);
    }

    inline std::string gotoNextNonAlpha(const std::string &text, size_t &pos) {
        const size_t start = pos;
        pos = text.find_first_of(NON_WHITE, pos);
        return text.substr(start, pos - start);
    }

    inline std::string goToNewLine(const std::string &text, size_t &pos) {
        const size_t start = pos;
        pos = text.find_first_of(NON_WHITE, pos);
        return text.substr(start, pos - start);
    }


    inline std::string nextAlNumWord(const std::string &data, size_t &pos) {
        const auto start = pos;
        for (; pos < data.size(); ++pos) {
            if (!isalnum(data.at(pos))) {
                return data.substr(start, pos - start);
            }
        }
        return data.substr(start);
    }

    inline std::string isOneOf(const std::string &data, const std::vector< std::string> &toCheck, size_t &pos) {
        const size_t start = pos;
        for (auto &check : toCheck) {
            if (start + check.size() >= data.size())
                continue;

            std::string ts = data.substr(start, check.size());
            if (ts == check) {
                pos += check.size();
                return ts;
            }
        }

        return "";
    }

};
