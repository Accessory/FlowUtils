#pragma once

#include <vector>
#include <string>
#include <map>
#include <functional>

class FlowInterface {
public:
    std::map<std::string, std::function<void(std::vector<std::string>, std::map<std::string, std::vector<std::string>>,
                                             size_t)>> actionMap;

    virtual ~FlowInterface() = default;
};

