#pragma once

#include <iostream>
#include <chrono>

template<class T = std::chrono::seconds>
class LifetimeClock {
public:
    LifetimeClock() = default;

    explicit LifetimeClock(bool doNotPrint) : DoNotPrint(doNotPrint) {}

    explicit LifetimeClock(std::string prefix, std::string suffix, bool doNotPrint = false) : Prefix(std::move(prefix)),
                                                                                              Suffix(std::move(suffix)),
                                                                                              DoNotPrint(doNotPrint) {}

    ~LifetimeClock() {
        if (!DoNotPrint) {
            PrintTimeRunning();
        }
    }

    void PrintTimeRunning() const {
        const auto end = std::chrono::system_clock::now();
        const auto elapsedTime = std::chrono::duration_cast<T>
                (end - start).count();

        std::cout << Prefix << elapsedTime << Suffix;
    }

    auto GetElapsedTime() const {
        const auto end = std::chrono::system_clock::now();
        return std::chrono::duration_cast<T>
                (end - start).count();
    }


    std::string Prefix = "Elapsed time: ";
    std::string Suffix = "\n";
    bool DoNotPrint;
private:
    std::chrono::time_point <std::chrono::system_clock> start = std::chrono::system_clock::now();

};

class LifetimeClock_Nanoseconds : public LifetimeClock<std::chrono::nanoseconds> {
public:
    explicit LifetimeClock_Nanoseconds() : LifetimeClock("Elapsed time: ", "ns\n") {}

    explicit LifetimeClock_Nanoseconds(bool doNotPrint) :
            LifetimeClock("Elapsed time: ", "ns\n", doNotPrint) {}

    explicit LifetimeClock_Nanoseconds(std::string prefix, std::string suffix, bool doNotPrint = false) :
            LifetimeClock(std::move(prefix), std::move(suffix), doNotPrint) {}
};

class LifetimeClock_Seconds : public LifetimeClock<std::chrono::seconds> {
public:
    explicit LifetimeClock_Seconds() : LifetimeClock("Elapsed time: ", "s\n") {}

    explicit LifetimeClock_Seconds(bool doNotPrint) :
            LifetimeClock("Elapsed time: ", "s\n", doNotPrint) {}

    explicit LifetimeClock_Seconds(std::string prefix, std::string suffix, bool doNotPrint = false) :
            LifetimeClock(std::move(prefix), std::move(suffix), doNotPrint) {}
};

class LifetimeClock_Micoseconds : public LifetimeClock<std::chrono::microseconds> {
public:
    explicit LifetimeClock_Micoseconds() : LifetimeClock("Elapsed time: ", "µs\n") {}

    explicit LifetimeClock_Micoseconds(bool doNotPrint) :
            LifetimeClock("Elapsed time: ", "s\n", doNotPrint) {}

    explicit LifetimeClock_Micoseconds(std::string prefix, std::string suffix, bool doNotPrint = false) :
            LifetimeClock(std::move(prefix), std::move(suffix), doNotPrint) {}
};

class LifetimeClock_Milliseconds : public LifetimeClock<std::chrono::milliseconds> {
public:
    explicit LifetimeClock_Milliseconds() : LifetimeClock("Elapsed time: ", "ms\n") {}

    explicit LifetimeClock_Milliseconds(bool doNotPrint) :
            LifetimeClock("Elapsed time: ", "s\n", doNotPrint) {}

    explicit LifetimeClock_Milliseconds(std::string prefix, std::string suffix, bool doNotPrint = false) :
            LifetimeClock(std::move(prefix), std::move(suffix), doNotPrint) {}
};

