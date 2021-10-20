#pragma once

#include <thread>
#include <memory>
#include <functional>
#include <mutex>
#include "MultiSemaphore.h"
#include <queue>
#include "PriorityWorker.h"
#include <functional>
#include <map>

class PriorityWorkerPool {
public:
    PriorityWorkerPool(const size_t &workerCount = std::thread::hardware_concurrency()) : workerCount(workerCount) {
        for (int i = 0; i < workerCount; ++i) {
            auto worker = std::make_shared<PriorityWorker>(workerId);
            workerMap[workerId] = worker;
            ++workerId;
            const auto onIdleCallback =
                    std::make_shared < std::function < void(PriorityWorker * worker) >> ([&](PriorityWorker *worker) {
                        std::unique_lock <std::mutex> lock(poolMutex);
                        toWaitFor.unlock();
                        idleWorker.push(workerMap.at(worker->getId()));
                        startWorker();
                    });
            worker->onIdle(onIdleCallback);
            const auto onStopCallback =
                    std::make_shared < std::function < void(PriorityWorker * worker) >> ([&](PriorityWorker *worker) {
                        toWaitFor.unlock();
                        startWorker();
                    });
            worker->onStop(onStopCallback);
            idleWorker.push(worker);
        }
    }

    void addTask(std::shared_ptr<std::function<void()>> function, size_t priority) {
        std::lock_guard guard(tasksMutex);
        toWaitFor.addLock();
        tasks.emplace(std::make_shared<PriorityTask>(function, priority));
    }

    void start() {
        std::unique_lock <std::mutex> lock(poolMutex, std::try_to_lock);
        if (lock.owns_lock()) {
            startWorker();
        }
    }

    void join() {
        isJoin = true;
        toWaitFor.wait();
        while (!tasks.empty()) {
            start();
            toWaitFor.wait();
            for (const auto& entry : workerMap) {
                entry.second->join();
            }
        }
    }

private:
    void startWorker() {
        std::lock_guard guard(tasksMutex);
        while (!tasks.empty() && !idleWorker.empty()) {
            try {
                const auto toRun = tasks.top();
                const auto worker = idleWorker.front();
                idleWorker.pop();
                worker->assignTask(toRun);
                tasks.pop();
            } catch (const std::system_error &e) {
                LOG_WARNING << "Code " << e.code()
                            << " meaning " << e.what() << '\n';
                tasks.pop();
                toWaitFor.unlock();
            }
        }
        if (isJoin && tasks.empty() && toWaitFor.count() == 0) {
            for (const auto& entry : workerMap) {
                entry.second->stop();
            }
        }
    }

    bool isJoin = false;
    std::atomic_size_t workerId;
    std::priority_queue <std::shared_ptr<PriorityTask>> tasks;
    MultiSemaphore toWaitFor;
    std::mutex poolMutex;
    std::mutex tasksMutex;
    size_t workerCount;
    std::queue <std::shared_ptr<PriorityWorker>> idleWorker;
    std::unordered_map <size_t, std::shared_ptr<PriorityWorker>> workerMap;
};