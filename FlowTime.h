#pragma once

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace FlowTime {
    inline std::string getCurrentDateTime() {
        const auto now = std::chrono::system_clock::now();
        const auto now_t = std::chrono::system_clock::to_time_t(now);
        return std::string(std::ctime(&now_t));
    }


    inline std::string getCurrentISO8601Time() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%FT%TZ");
        return  oss.str();
    }
}