#pragma once

#include <string>
#include <array>
#include <vector>
#include <cstring>



namespace FlowCParser {
    using namespace std;
    inline string goToNextLine(unsigned char *&text) {
        unsigned char *start = text;
        size_t pos = 0;
        if (text[pos] == '\n' || (text[pos] == '\r' && text[++pos] == '\n'))
            ++pos;

        text = text + pos;
        return string(text, text + pos);
    }

    inline bool isEmpty(unsigned char *&text){
        return text == nullptr || *text == '\0';
    }

    inline string returnString(unsigned char *&start, unsigned char *&endP) {
        if (isEmpty(endP))
            return string((char *) start);
        return string(start, endP);
    }

    inline string goToNewLine(unsigned char *&text) {
        unsigned char *start = text;
        text = (unsigned char*) strpbrk((char*)text, "\n\r");
        return returnString(start, text);
    }

    inline string goTo(unsigned char *&text, char c) {
        unsigned char *start = text;
        text = (unsigned char*) strchr((char*)text, c);
        return returnString(start, text);
    }

    inline string goTo(unsigned char *&text,const string& str) {
        unsigned char *start = text;
        text = (unsigned char*) strstr((char*)text, str.c_str());
        return returnString(start, text);
    }

    inline vector<char> returnVector(unsigned char *&start, unsigned char *&endP) {
        if (isEmpty(endP))
            return vector<char> (start, start + strlen((char*)start));
        return vector<char>(start, endP);
    }

    inline vector<char> goToV(unsigned char *&text,const string& str) {
        unsigned char *start = text;
        text = (unsigned char*) strstr((char*)text, str.c_str());
        return returnVector(start, text);
    }

    inline string gotoNextNonWhite(unsigned char *&text) {
        unsigned char *start = text;
        while (*text++ != '\0') {
            if (!isblank(*text))
                break;
        }
        return returnString(start, text);
    }

    inline string gotoNextNonAlpha(unsigned char *&text) {
        unsigned char *start = text;
        text = (unsigned char*) strpbrk((char*)text, " \n\r");
        return returnString(start, text);
    }

    inline string goToOne(unsigned char *&text, const string &goToOne) {
        unsigned char *start = text;
        text = (unsigned char*)  strpbrk((char*)text, goToOne.c_str());
        return returnString(start, text);

    }

    inline bool isDoubleNewLine(unsigned char *&text) {
        return (*text == '\n' && *(++text) == '\n') ||
               (*text == '\r' && *(++text) == '\n' && *(++text) == '\r' && *(++text) == '\n');
    }

    inline string goToEnd(unsigned char *&text) {
        return string((char*)text);
    }

    inline string between(unsigned char *&text, const char startChar, const char endChar) {
        char *posStart = strchr((char*)text, startChar);
        if (posStart == nullptr)
            return "";
        char *posEnd = strchr(posStart + 1, endChar);
        if (posEnd == nullptr)
            return "";

        return string(posStart, posEnd);
    }

    inline vector<char>::iterator findLastData(vector<char> &data) {
        auto rtn = data.end() - 1;
        if(*rtn == '-')
            --rtn;
        if(*rtn == '-')
            --rtn;
        if(*rtn == '\n')
            --rtn;
        if(*rtn == '\r')
            --rtn;
        return rtn;
    }

};
