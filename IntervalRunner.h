#pragma once

#include <functional>
#include <thread>
#include <memory>
#include <utility>
#include <chrono>

class IntervalRunner {
public:

    IntervalRunner(std::function<void()> toRun, std::chrono::milliseconds runEvery,
                   std::chrono::milliseconds firstDelay,
                   bool singleRun = false, bool alwaysWait = false) : toRun(std::move(toRun)), runEvery(runEvery),
                                                                      firstDelay(firstDelay),
                                                                      singleRun(singleRun), alwaysWait(alwaysWait) {}
    ~IntervalRunner(){
        Stop();
        Join();
    }
    void Run() {
        running = true;
        mainThread = std::make_unique<std::thread>([&] {
            std::this_thread::sleep_for(firstDelay);
            do {
                this->start = std::chrono::system_clock::now();
                this->toRun();
                this->end = std::chrono::system_clock::now();
                this->nextRun = this->alwaysWait ? this->runEvery :
                                std::chrono::duration_cast<std::chrono::milliseconds>(
                                        this->runEvery + this->start - this->end);
                std::this_thread::sleep_for(this->nextRun);
            } while (!singleRun);
            this->running = false;
        });
    }

    void Stop() {
        singleRun = true;
    }

    void Join() {
        mainThread->join();
    }

    bool isRunning() {
        return this->running;
    }

private:
    std::function<void()> toRun;
    std::chrono::milliseconds runEvery;
    std::chrono::milliseconds firstDelay;
    std::chrono::milliseconds nextRun;
    bool running = false;
    bool singleRun = false;
    bool alwaysWait = false;
    std::unique_ptr<std::thread> mainThread;
    std::chrono::time_point<std::chrono::system_clock> start, end;
};
