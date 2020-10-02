#pragma once


#include <vector>
#include <map>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <regex>

namespace FlowString {

    inline std::vector<std::string> splitToStringVector(std::string line, std::string delimiter) {
        std::vector<std::string> rtn;
        boost::split(rtn, line, boost::is_any_of(delimiter));

        return rtn;
    }

    inline std::vector<std::string> splitNotEmpty(std::string line, std::string delimiter) {
        std::vector<std::string> tmp;
        boost::split(tmp, line, boost::is_any_of(delimiter));
        std::vector<std::string> rtn;
        for (std::string l : tmp) {
            boost::trim(l);
            if (!l.empty()) rtn.emplace_back(l);
        }
        return rtn;
    }

    inline std::string findFromLast(const std::string &text, const std::string &delimiter, const int &offset) {
        std::size_t found = text.find_last_of(delimiter);
        found += offset;
        return text.substr(found);
    }

    inline bool isNumber(const std::string &text) {
        return !text.empty() && std::find_if(text.begin(),
                                             text.end(),
                                             [](char c) { return !std::isdigit(c) || c == '.' || c == ','; }) ==
                                text.end();
    }

    inline std::string join(const std::vector<std::string>& toJoin, const std::string& delimiter ){
        return boost::join(toJoin, delimiter);
    }

    inline std::string format(const std::string& text, size_t number) {
        boost::format rtn(text);
        rtn % number;
        return rtn.str();
    }

    inline std::string format(const std::string& text, int number) {
        boost::format rtn(text);
        rtn % number;
        return rtn.str();
    }

    inline std::string format(const std::string& text, std::string input) {
        boost::format rtn(text);
        rtn % input;
        return rtn.str();
    }

    inline std::string subString(const std::string &text, const std::string &start, const std::string &end) {
        size_t startpos = text.find(start);
        if (startpos == std::string::npos) return "";
        startpos = startpos + start.length();
        size_t endpos = text.find(end, startpos);

        if (endpos == std::string::npos) return "";

        return text.substr(startpos, endpos - startpos);
    }

    inline void replaceAll(std::string &str, const std::string &from, const std::string &to) {
        if (from.empty()) return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    inline size_t findRegex(const std::string &text, const std::string &search, const int &position) {
        std::smatch m;
        std::regex e(search);
        std::string s = text.substr(position);
        if (regex_search(s, m, e)) return m.position() + position;
        return std::string::npos;
    }

    inline std::vector<std::string>
    subStringAllRegEx(const std::string &text, const std::string &from, const std::string &to) {
        size_t position = 0;
        std::vector<std::string> rtn;

        size_t start;
        size_t ende;

        do {
            start = findRegex(text, from, position);
            if (start == std::string::npos) break;
            ende = findRegex(text, to, start + from.length());
            rtn.push_back(text.substr(start, ende - start));
            position = ende + to.length();
        } while (position != std::string::npos);

        return rtn;
    }

    inline std::vector<std::string>
    subStringAll(const std::string &text, const std::string &from, const std::string &to) {
        size_t position = 0;
        std::vector<std::string> rtn;

        size_t start;
        int ende;

        do {
            start = text.find(from, position);
            if (start == std::string::npos) break;
            start += from.length();
            ende = text.find(to, start + from.length());
            rtn.push_back(text.substr(start, ende - start));
            position = ende + to.length();
        } while (position != std::string::npos);

        return rtn;
    }

    inline void stringToFile(const std::string &file, const std::string &text) {
        std::ofstream myfile;
        myfile.open(file);
        myfile << text;
        myfile.close();
    }

    inline std::vector<std::string>
    getAllFromRegexGroup(const std::string &text, const std::string &search, const size_t group) {
        std::vector<std::string> rtn;
        size_t position = 0;
        std::regex rgx(search);
        std::smatch mtch;
        do {
            std::string toSearch = text.substr(position);
            if (!regex_search(toSearch, mtch, rgx)) break;
            rtn.push_back(mtch[group]);
            position += mtch.position() + mtch.length();
        } while (position != std::string::npos);

        return rtn;
    }

    inline std::map<std::string, std::string>
    getAllFromRegexGroups(const std::string &text, const std::string &search, const size_t group,
                          const size_t group2) {
        std::map<std::string, std::string> rtn;
        size_t position = 0;
        std::regex rgx(search);
        std::smatch mtch;
        do {
            std::string toSearch = text.substr(position);
            if (!regex_search(toSearch, mtch, rgx)) break;
            rtn[mtch[group2]] = (mtch[group]);
            position += mtch.position() + mtch.length();
        } while (position != std::string::npos);

        return rtn;
    }

    inline std::string getFromRegexGroup(const std::string &text, const std::string &search, const size_t group) {
        std::vector<std::string> rtn;
        size_t position = 0;
        std::regex rgx(search);
        std::smatch mtch;
        std::string toSearch = text.substr(position);
        if (!regex_search(toSearch, mtch, rgx)) return "";
        return mtch[group];
    }

    inline std::map<std::string, std::string> findKeyValue(const std::string &text, const std::string &search) {
        std::map<std::string, std::string> rtn;

        size_t position = 0;
        std::regex rgx(search);
        std::smatch mtch;
        do {
            std::string toSearch = text.substr(position);
            if (!regex_search(toSearch, mtch, rgx)) break;
            rtn[mtch[1]] = mtch[2];
            position += mtch.position() + mtch.length();
        } while (position != std::string::npos);

        return rtn;
    }


    inline void trim(std::string &text) {
        text.erase(text.begin(), std::find_if(text.begin(), text.end(), [](int ch) {
            return !std::isspace(ch);
        }));
        text.erase(std::find_if(text.rbegin(), text.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), text.end());
    }

    inline void setUnicode() {
        setlocale(LC_ALL, "de_DE.UTF-8");
    }

    inline void toUpper(std::string &text) {
        boost::algorithm::to_upper(text);
    }

    inline void toLower(std::string &text) {
        boost::algorithm::to_lower(text);
    }

    inline void underscoreToCamelCase(std::string &text, bool firstLetter = false) {
        if (text.empty())
            return;

        if (firstLetter)
            text.replace(0, 1, 1, static_cast<char>(toupper((text.at(0)))));

        auto pos = text.find('_');
        while (pos != std::string::npos) {
            if (isalpha(text.at(pos + 1)))
                text.replace(pos, 2, 1, static_cast<char>(toupper((text.at(pos + 1)))));
            pos = text.find('_', ++pos);
        }
    }

    inline void slashToCamelCase(std::string &text, bool firstLetter = false) {
        if (text.empty())
            return;

        if (firstLetter)
            text.replace(0, 1, 1, static_cast<char>(toupper((text.at(0)))));

        auto pos = text.find('/');
        while (pos != std::string::npos) {
            if (isalpha(text.at(pos + 1)))
                text.replace(pos, 2, 1, static_cast<char>(toupper((text.at(pos + 1)))));
            pos = text.find('/', ++pos);
        }
    }

    inline void dotToCamelCase(std::string &text, bool firstLetter = false) {
        if (text.empty())
            return;

        if (firstLetter)
            text.replace(0, 1, 1, static_cast<char>(toupper((text.at(0)))));

        auto pos = text.find('.');
        while (pos != std::string::npos) {
            if (isalpha(text.at(pos + 1)))
                text.replace(pos, 2, 1, static_cast<char>(toupper((text.at(pos + 1)))));
            pos = text.find('.', ++pos);
        }
    }

    inline void firstUpper(std::string &text) {
        auto pos = text.find('_');
        while (pos != std::string::npos) {
            if (isalpha(text.at(pos + 1)))
                text.replace(pos, 2, 1, static_cast<char>(toupper((text.at(pos + 1)))));
            pos = text.find('_', ++pos);
        }
    }

    inline std::string subStrAt(const std::string &text, const std::string &find, const size_t offset) {
        auto pos = text.find(find);
        if (pos == std::string::npos)
            return "";
        pos += offset;
        if (pos > text.size())
            return "";
        return text.substr(pos);

    }

    inline void removeNonAlphaNum(std::string &text) {
        for (size_t i = 0; i < text.length(); ++i) {
            if (!isalnum(text.at(i))) {
                text.erase(i--, 1);
            }
        }
    }

    inline bool alphaNumSort(const std::string &one, const std::string &two) {
        size_t minLength = (std::min)(one.size(), two.size());

        for (size_t i = 0; i < minLength; ++i) {
            if (std::isdigit(one.at(i)) && std::isdigit(two.at(i))) {
                auto first = std::stoul(one.substr(i));
                auto second = std::stoul(two.substr(i));
                if (first == second) {
                    while (i + 1 < minLength && std::isdigit(one.at(i + 1)))
                        ++i;
                    continue;
                }
                return first < second;
            }
            if (one.at(i) == two.at(i))
                continue;

            return one.at(i) < two.at(i);
        }
        return one.size() < two.size();
    }
};



