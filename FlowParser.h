#pragma once

#include <string>


namespace FlowParser {
    inline size_t findLastData(const std::string &text, size_t pos) {
            --pos;
        if(text.at(pos) == '-')
            --pos;
        if(text.at(pos) == '-')
            --pos;
        if(text.at(pos) == '\n')
            --pos;
        if(text.at(pos) == '\r')
            --pos;
        ++pos;

        return pos;
    }

    inline std::string goToNextLine(const std::string &text, size_t &pos) {
        size_t start = pos;
        if (text[pos] == '\n' || (text[pos] == '\r' && text[++pos] == '\n'))
            ++pos;
        return text.substr(start, pos - start);
    }

    inline std::string goToNewLine(const std::string &text, size_t &pos) {
        size_t start = pos;
        pos = text.find_first_of("\n\r", pos);
        return text.substr(start, pos - start);
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

    inline std::string goToOne(const std::string &text, const std::string &goToOne, size_t &pos) {
        size_t start = pos;
        pos = text.find_first_of(goToOne, pos);
        return text.substr(start, pos - start);
    }

    inline bool isDoubleNewLine(const std::string &text, size_t &pos) {
        return (text[pos] == '\n' && text[++pos] == '\n') ||
               (text[pos] == '\r' && text[++pos] == '\n' && text[++pos] == '\r' && text[++pos] == '\n');
    }

    inline std::string goToEnd(const std::string &text, size_t &pos) {
        size_t start = pos;
        pos = std::string::npos;
        return text.substr(start);
    }

    inline std::string goToPrevLine(const std::string &text, size_t &pos) {
        size_t end = pos;
        if (text[--pos] == '\n' && text[--pos] == '\r')
            --pos;
        return text.substr(pos, end - pos);
    }

    inline std::string between(const std::string &text, const std::string &start, const std::string &end) {
        size_t posStart = text.find(start);
        if (posStart == std::string::npos)
            return "";
        posStart += start.length();
        size_t posEnd = text.find(end, posStart);
        if (posEnd == std::string::npos)
            return "";

        return text.substr(posStart, posEnd - posStart);
    }

};
