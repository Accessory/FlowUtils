#pragma once

#include <thread>
#include <memory>
#include <functional>
#include <mutex>
#include "MultiSemaphore.h"
#include <queue>
#include "FlowLog.h"
#include "Worker.h"
#include <functional>
#include <map>

class WorkerPool {
public:
    WorkerPool(const size_t &workerCount = std::thread::hardware_concurrency()) : workerCount(workerCount) {
        for (int i = 0; i < workerCount; ++i) {
            auto worker = std::make_shared<Worker>(workerId);
            workerMap[workerId] = worker;
            ++workerId;
            const auto onIdleCallback = std::make_shared<std::function<void(Worker *worker)>>([&](Worker *worker) {
                std::unique_lock<std::mutex> lock(poolMutex);
                toWaitFor.unlock();
                idleWorker.push(workerMap.at(worker->getId()));
                startWorker();
//                LOG_INFO << "Idle: "<< "Locks " << toWaitFor.count() << " Tasks " << tasks.size();
            });
            worker->onIdle(onIdleCallback);
            const auto onStopCallback = std::make_shared<std::function<void(Worker *worker)>>([&](Worker *worker) {
                toWaitFor.unlock();
                startWorker();
                LOG_INFO << "Stop: "<< "Locks " << toWaitFor.count() << " Tasks " << tasks.size();
            });
            worker->onStop(onStopCallback);
            idleWorker.push(worker);
        }
    }

    void addTask(std::shared_ptr<std::function<void()>> function) {
        std::lock_guard guard(tasksMutex);
        tasks.emplace(function);
        toWaitFor.addLock();
    }

    void start() {
        std::unique_lock<std::mutex> lock(poolMutex, std::try_to_lock);
        if (lock.owns_lock()) {
            startWorker();
        }
    }

    void join() {
        isJoin = true;
        while (!tasks.empty()) {
            start();
            toWaitFor.wait();
            for (const auto entry : workerMap){
                entry.second->join();
            }
        }
    }

private:
    void startWorker() {
        std::lock_guard guard(tasksMutex);
        while (!tasks.empty() && !idleWorker.empty()) {
            try {
                const auto toRun = tasks.front();
                tasks.pop();
                const auto worker = idleWorker.front();
                idleWorker.pop();
                worker->assignTask(toRun);
            } catch (const std::system_error &e) {
                LOG_WARNING << "Code " << e.code()
                            << " meaning " << e.what() << '\n';
                tasks.pop();
                toWaitFor.unlock();
            }
        }
        if(isJoin && tasks.empty() && toWaitFor.count() == 0){
            for (const auto entry : workerMap) {
                entry.second->stop();
            }
        }
    }
    bool isJoin = false;
    std::atomic_size_t workerId;
    std::queue<std::shared_ptr<std::function<void()>>> tasks;
    MultiSemaphore toWaitFor;
    std::mutex poolMutex;
    std::mutex tasksMutex;
    size_t workerCount;
    std::queue<std::shared_ptr<Worker>> idleWorker;
    std::map<size_t, std::shared_ptr<Worker>> workerMap;
};