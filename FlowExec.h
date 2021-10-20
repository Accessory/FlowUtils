#pragma once

#include <string>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <array>

namespace FlowExec {
    inline std::string open(const std::string &command) {
        std::array<char, 512> buffer;
        std::stringstream result;
#ifdef _MSC_VER
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
#else
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
#endif
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result << buffer.data();
        }
        return result.str();
    }

    inline int exec(const std::string &command) {
        return system(command.c_str());
    }
};
