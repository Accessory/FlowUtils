#pragma once

#ifdef LOG_BOOST_DLL

#include <boost/dll.hpp>

#endif

#include <chrono>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <mutex>
#include <fstream>
#include <memory>

#ifdef ERROR
#undef ERROR
#endif

namespace logging {
    enum severity {
        TRACE, DEBUG, INFO, WARNING, ERROR, FATAL
    };

    class config {
    public:
        void setLevel(logging::severity level) {
            this->level = level;
        }

        void setLogFile(const std::string &logFile) {
            _logFile = std::make_shared<std::ofstream>(logFile.c_str());
        }

        logging::severity getLevel() const {
            return level;
        }

        std::shared_ptr<std::ofstream> getLogFile() {
            return _logFile;
        }

        static std::shared_ptr<logging::config> getInstance() {
            if (!_instance)
                _instance = std::make_shared<logging::config>();

            return _instance;
        }

        config(const config &) = delete;

        constexpr config() :
                level(INFO), _logFile(nullptr) {};

        ~config() {
//            delete _instance;
            if (_logFile != nullptr) {
                _logFile->close();
//                delete _logFile;
            }
        }

    private:
        static std::shared_ptr<config> _instance;
        logging::severity level;
        std::shared_ptr<std::ofstream> _logFile;
    };

    inline std::shared_ptr<logging::config> logging::config::_instance;

    static std::shared_ptr<logging::config> getConfig() {
        return config::getInstance();
    };

    static std::mutex loggingMutex;

    struct LOGGER {
        std::stringstream buffer;
        severity level;
        severity currentLogLevel;
        std::shared_ptr<std::ofstream> logFile;

        ~LOGGER() {
            std::lock_guard<std::mutex> lock(loggingMutex);
            if (currentLogLevel <= level) {
#ifdef _WIN32
                if (level == severity::ERROR) {
                    std::cerr << buffer.str() << "\r\n";
                } else {
                    std::cout << buffer.str() << "\r\n";
                }
#else
                if (level == severity::ERROR) {
                    std::cerr << buffer.str() << '\n';
                } else {
                    std::cout << buffer.str() << '\n';
                }
#endif
//                std::cout << buffer.str() << std::endl;
                if (logFile != nullptr && logFile->is_open()) {
                    *logFile << buffer.str() << std::endl << std::flush;
                }
            };
//            delete buffer;
        }

        LOGGER &operator<<(std::ostream &(*txt)(std::ostream &)) {
//            std::cout << "ostream Function: " << txt << " - "  << &logging::level << " - " << logging::level << " test with " << level << endl;
            if (currentLogLevel <= level) buffer << txt;
            return *this;
        }

        template<class T>
        LOGGER &operator<<(const T &txt) {
            if (currentLogLevel <= level) buffer << txt;
            return *this;
        }
    };

    static std::unique_ptr<LOGGER> logger(severity level, std::shared_ptr<logging::config> config) {
        std::unique_ptr<LOGGER> logInst = std::make_unique<LOGGER>();
        logInst->level = level;
        logInst->currentLogLevel = config->getLevel();
        logInst->logFile = config->getLogFile();
        return logInst;
    };

    static std::unique_ptr<LOGGER> log(logging::severity severity) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::unique_ptr<LOGGER> logInst = logger(severity, logging::getConfig());
        *logInst << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

        switch (severity) {
            case TRACE: {
                *logInst << " [trace] - ";
                break;
            }
            case DEBUG: {
                *logInst << " [debug] - ";
                break;
            }
            case INFO: {
                *logInst << " [info] - ";
                break;
            }
            case WARNING: {
                *logInst << " [warning] - ";
                break;
            }
            case ERROR: {
                *logInst << " [error] - ";
                break;
            }
            case FATAL: {
                *logInst << " [fatal] - ";
                break;
            }
        }
        return logInst;
    }

    static inline void setLogLevel(logging::severity level) {
        logging::getConfig()->setLevel(level);
    };

    static inline void setLogFile(const std::string &logFile) {
        logging::getConfig()->setLogFile(logFile);
    };
}  // namespace logging

// ===== log macros =====
#define LOG_TRACE *logging::log(logging::TRACE)
#define LOG_DEBUG *logging::log(logging::DEBUG)
#define LOG_INFO *logging::log(logging::INFO)
#define LOG_WARNING *logging::log(logging::WARNING)
#define LOG_ERROR *logging::log(logging::ERROR)
#define LOG_FATAL *logging::log(logging::FATAL)

//#ifdef LOG_BOOST_DLL
//BOOST_DLL_ALIAS(logging::setLogLevel,  // <-- this function is exported with...
//                setLogLevel            // <-- ...this alias name
//)
//
//BOOST_DLL_ALIAS(logging::setLogFile,  // <-- this function is exported with...
//                setLogFile            // <-- ...this alias name
//)
//#endif