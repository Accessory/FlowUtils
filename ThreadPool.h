#pragma once

#include <thread>
#include <vector>
#include <functional>
#include "MultiSemaphore.h"
#include "Semaphore.h"
#include <queue>
#include <atomic>
#include "FlowLog.h"
#include <memory>

class ThreadPool {
public:

    ThreadPool(const size_t &threadLimit = std::thread::hardware_concurrency()) {
        this->threadLimit = threadLimit;
    }

    void addFunction(std::shared_ptr<std::function<void()>> function) {
        std::lock_guard guard(functionsMutex);
        toWaitFor.addLock();
        functions.emplace(function);
    }

    void start() {
        std::unique_lock<std::mutex>lock(poolMutex, std::try_to_lock);
        if (lock.owns_lock()) {
            startThreads();
        }
    }

    void join() {
        while (!functions.empty() || runningThreads > 0 || toWaitFor.count() != 0) {
            start();
            toWaitFor.wait();
        }
    }

    size_t threadLimit;

private:
    void startThreads() {
        while (runningThreads < threadLimit && !functions.empty()) {
            try {
                std::lock_guard guard(functionsMutex);
                auto toRun = functions.front();
                std::thread([&, toRun] {
                    ++runningThreads;
                    (*toRun)();
                    --runningThreads;
                    start();
                    toWaitFor.unlock();
                }).detach();
                functions.pop();
            } catch (const std::system_error &e) {
                std::cout << "Code " << e.code()
                            << " meaning " << e.what() << '\n';
                functions.pop();
                --runningThreads;
            }
        }
    }

    std::queue<std::shared_ptr<std::function<void()>>> functions;

    std::atomic_size_t runningThreads = {0};
    MultiSemaphore toWaitFor;
    std::mutex poolMutex;
    std::mutex functionsMutex;
};
