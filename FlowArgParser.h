#pragma once

#include <set>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <cstring>
#include <fstream>
#include "FlowParser.h"

struct FlowValue {
    std::string value;
    std::vector<std::string> list;
    bool isSet;

    FlowValue() {}

    ~FlowValue() {}
};

struct FlowOption {
    FlowOption(const std::string &name, const std::string &description, const bool required, const bool isFlag,
               const bool isList) {
        this->var1 = name;
        this->var2 = name;
        this->description = description;
        this->required = required;
        this->isFlag = isFlag;
        this->isList = isList;
        this->isIndex = true;
    }

    FlowOption(const char *name) {
        if (nullptr == name)
            return;
        auto len = strlen(name);
        this->var1 = std::string(name, len);
        this->var2 = std::string(name, len);
    }

    FlowOption(const std::string &name) {
        this->var1 = name;
        this->var2 = name;
    }

    FlowOption(const std::string &longName, const std::string &shortName, const std::string &description,
               const bool required, const bool isFlag, const bool isList) {
        this->var1 = longName;
        this->var2 = shortName;
        this->description = description;
        this->required = required;
        this->isFlag = isFlag;
        this->isList = isList;
        this->isIndex = false;
    }

    ~FlowOption() {}

    std::string var1;
    std::string var2;
    std::string description;
    bool required;
    bool isFlag;
    bool isList;
    bool isIndex;
    bool mark = false;

    bool operator<(const FlowOption &flowOption) const {
        if (this->var1 == flowOption.var1 || this->var2 == flowOption.var2)
            return 0;
        return this->var1 < flowOption.var1 || this->var2 < flowOption.var2;
    }
};

//struct Compare {
//    using is_transparent = void;
//
//    bool operator()(const std::shared_ptr<FlowOption> &a, const std::shared_ptr<FlowOption> &b) const {
//        return *a < *b;
//    }
//
//    bool operator()(const std::shared_ptr<FlowOption> &a, const std::string &b) const {
//        return *a < b;
//    }
//};


class FlowArgParser {
public:
    void addIndexOption(const std::string &name, const std::string &description, const bool required = false) {
        _indexOptions.emplace_back(std::make_shared<FlowOption>(name, description, required, false, false));

    }

    void addIndexList(const std::string &name, const std::string &description,
                      const bool required = false) {
        _indexOptions.emplace_back(std::make_shared<FlowOption>(name, description, required, false, true));
    }

    void addParameterOption(const std::string &shortName, const std::string &longName, const std::string &description,
                            const bool required = false) {
        _options.emplace(std::make_shared<FlowOption>(shortName, longName, description, required, false, false));
    }

    void addFlagOption(const std::string &shortName, const std::string &longName, const std::string &description,
                       const bool required = false) {
        _options.emplace(std::make_shared<FlowOption>(shortName, longName, description, required, true, false));
    }

    void addListOption(const std::string &shortName, const std::string &longName, const std::string &description,
                       const bool required = false) {
        _options.emplace(std::make_shared<FlowOption>(shortName, longName, description, required, false, true));
    }

    int parse(const std::string &args) {
        int argc = 0;
        size_t pos = 0;
        size_t start;
        size_t indexPos = 0;

        while (pos != std::string::npos) {
            ++argc;
            pos = args.find_first_not_of(' ', pos);
            start = pos;
            char ending = args.at(pos) == '"' ? '"' : ' ';
            pos = args.find(ending, ++pos);
            std::string arg = pos == std::string::npos ? args.substr(start) : args.substr(start, pos - start);

            std::string err;
            auto fo = getFlowOption(arg.c_str(), indexPos, err);
            if (!err.empty())
                return argc;

            if (!fo->isIndex && !fo->isFlag) {
                ++argc;
                pos = args.find_first_not_of(' ', pos);
                start = pos;
                char ending = args.at(pos) == '"' ? '"' : ' ';
                pos = args.find(ending, ++pos);
                arg = pos == std::string::npos ? args.substr(start) : args.substr(start, pos - start);
            }

            if (fo->isFlag) {
                _values.emplace(fo, FlowValue()).first->second.isSet = true;
                continue;
            }

            if (fo->isList) {
                _values[fo].list.emplace_back(arg);
                continue;
            }

            _values.emplace(fo, FlowValue()).first->second.value = arg;
        }

        return 0;
    }

    int parse(const int argc, char *argv[]) {
        size_t indexPos = 0;
        for (int i = 1; i < argc; ++i) {
            std::string err;
            auto fo = getFlowOption(argv[i], indexPos, err);
            if (!err.empty())
                return i;

            if (!fo->isIndex && !fo->isFlag)
                ++i;

            if (fo->isFlag) {
                _values.emplace(fo, FlowValue()).first->second.isSet = true;
                continue;
            }

            if (fo->isList) {
                if (fo->mark)
                    _values[fo].list.clear();

                _values[fo].list.emplace_back(argv[i], strlen(argv[i]));
                continue;
            }

            _values.emplace(fo, FlowValue()).first->second.value = argv[i];
        }
        return 0;
    }

    bool parseFile(const std::string &file) {
        std::string line;
        std::ifstream ifs(file);
        while (!ifs.eof()) {
            std::getline(ifs, line);
            size_t pos = 0;
            FlowParser::gotoNextNonWhite(line, pos);
            if (line.at(pos) == '#')
                continue;

            auto key = FlowParser::goToOne(line, " :\n\r", pos);
            if (pos > line.size())
                continue;

            auto fo = getFlowOption(key);
            if (fo == nullptr)
                continue;

            if (fo->isList) {
                ++pos;
                std::vector<std::string> valueList;
                while (pos < line.size()) {
                    FlowParser::gotoNextNonWhite(line, pos);
                    if (pos < line.size()) {
                        auto value = FlowParser::gotoNextNonAlpha(line, pos);
                        valueList.emplace_back(std::move(value));
                    }
                }
                _values[fo].list = std::move(valueList);
            } else {

                FlowParser::gotoNextNonWhite(line, pos);

                if (line.at(pos) != ':')
                    continue;

                FlowParser::gotoNextNonWhite(line, ++pos);
                auto value = line.substr(pos);
                _values[fo].value = std::move(value);
            }
        }

        return true;
    }

    bool hasOption(const std::string &name) {
        return findInValues(name) != _values.end();
    }

    std::string getString(const std::string &name) {
        auto item = findInValues(name);
        if (item == _values.end())
            return "";

        return item->second.value;
    }

    float getFloat(const std::string &name) {
        auto val = getString(name);
        return std::stof(val);
    }

    bool getBool(const std::string &name) {
        auto item = findInValues(name);
        if (item == _values.end())
            return false;
        if(item->first->isFlag){
            return item->second.isSet;
        }
        const auto& val = item->second.value;
        return val == "True" || val == "true";
    }

    size_t getSizeT(const std::string &name) {
        auto val = getString(name);
        return std::stoul(val);
    }

    std::vector<std::string> getList(const std::string &name) {
        auto item = findInValues(name);
        if (item == _values.end())
            return std::vector<std::string>();

        return item->second.list;
    }

    void markList(const std::string &name) {
        auto item = findInValues(name);
        if (item != _values.end())
            item->first->mark = true;
    }

    bool hasFlag(const std::string &name) {
        auto item = findInValues(name);
        if (item == _values.end())
            return false;

        return item->second.isSet;
    }

    bool hasRequiredOptions() {
        for (auto &item : _options) {
            if (item->required) {
                if (!_values.count(item))
                    return false;
            }
        }

        for (auto &item : _indexOptions) {
            if (item->required) {
                if (!_values.count(item))
                    return false;
            }
        }

        return true;
    }


private:
    std::map<std::shared_ptr<FlowOption>, FlowValue>::iterator findInValues(const std::string &name) {
        for (auto itr = _values.begin(); itr != _values.end(); ++itr) {
            if (itr->first->var1 == name || itr->first->var2 == name)
                return itr;
        }
        return _values.end();
    }

    std::set<std::shared_ptr<FlowOption>>::iterator findInOptions(const std::string &name) {
        for (auto itr = _options.begin(); itr != _options.end(); ++itr) {
            if (itr->get()->var1 == name || itr->get()->var2 == name)
                return itr;
        }
        return _options.end();
    }

    std::shared_ptr<FlowOption> getFlowOption(const std::string &key) {
        auto option = std::find_if(_options.begin(), _options.end(), [&](auto &o) {
            return o->var1 == key || o->var2 == key;
        });

        if (option != _options.end()) {
            return *option;
        }

        auto otheroption = std::find_if(_indexOptions.begin(), _indexOptions.end(), [&](auto &o) {
            return o->var1 == key || o->var2 == key;
        });

        if (otheroption != _indexOptions.end()) {
            return *otheroption;
        }

        return nullptr;

    }

    std::shared_ptr<FlowOption> getFlowOption(const char *argv, size_t &indexPos, std::string &err) {

        if (argv[0] == '-') {
            auto start = argv[1] == '-' ? &argv[2] : &argv[1];
            auto item = findInOptions(start);
            if (item == _options.end()) {
                err = "Could not find Option";
                return nullptr;
            }
            return *item;
        }

        if (indexPos >= _indexOptions.size()) {
            err = "To many Options";
            return nullptr;
        }

        auto &idxItem = _indexOptions.at(indexPos);
        if (!idxItem->isList) {
            ++indexPos;
        }
        return idxItem;
    }

    std::set<std::shared_ptr<FlowOption>> _options;
    std::vector<std::shared_ptr<FlowOption>> _indexOptions;
    std::map<std::shared_ptr<FlowOption>, FlowValue> _values;
};