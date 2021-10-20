#include <string>
#include <map>
#include "FlowParser.h"

class FlowEscape {
public:

    static inline void UrlDecode(std::string &url) {
        size_t pos = 0;
        FlowParser::goTo(url, "%", pos);
        while (pos != std::string::npos) {
            auto itr = _decodeUrlMap.find(url.substr(pos, 3));
            if (itr != _decodeUrlMap.end()) {
                url.replace(pos, 3, itr->second);
            }
            FlowParser::goTo(url, "%", ++pos);
        }
    }

    static inline void UrlEncode(std::string &url) {
        size_t pos = 0;
        FlowParser::goToOne(url, encodeString, pos);
        while (pos != std::string::npos) {
            auto itr = _encodeUrlMap.find(url.substr(pos, 1));
            if (itr != _encodeUrlMap.end()) {
                url.replace(pos, 1, itr->second);
            }
            FlowParser::goToOne(url, encodeString, ++pos);
        }
    }

private:
    static inline const std::string encodeString = " !%+,;<=>?@[\\]^`{|}~";
    static inline const std::unordered_map<std::string, std::string> _decodeUrlMap = {
            {"%20", " "},
            {"%22", "!"},
            {"%25", "%"},
            {"%2B", "+"},
            {"%2C", ","},
            {"%2F", "/"},
            {"%3A", ":"},
            {"%3B", ";"},
            {"%3C", "<"},
            {"%3D", "="},
            {"%3E", ">"},
            {"%3F", "?"},
            {"%40", "@"},
            {"%5B", "["},
            {"%5C", "\\"},
            {"%5D", "]"},
            {"%5E", "^"},
            {"%60", "`"},
            {"%7B", "{"},
            {"%7C", "|"},
            {"%7D", "}"},
            {"%7E", "~"},
            {"%7F", ""}
    };
    static inline const std::unordered_map<std::string, std::string> _encodeUrlMap = {
            {" ",  "%20"},
            {"!",  "%22"},
            {"%",  "%25"},
            {"+",  "%2B"},
            {",",  "%2C"},
            {";",  "%3B"},
            {"<",  "%3C"},
            {"=",  "%3D"},
            {">",  "%3E"},
            {"?",  "%3F"},
            {"@",  "%40"},
            {"[",  "%5B"},
            {"\\", "%5C"},
            {"]",  "%5D"},
            {"^",  "%5E"},
            {"`",  "%60"},
            {"{",  "%7B"},
            {"|",  "%7C"},
            {"}",  "%7D"},
            {"~",  "%7E"},
    };
};