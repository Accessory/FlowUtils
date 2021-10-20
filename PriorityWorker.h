#pragma once

#include <thread>
#include <functional>
#include "Semaphore.h"

struct PriorityTask {

    PriorityTask(const std::shared_ptr<std::function<void()>> &function,
                 size_t priority) : toRun(function),
                                    priority(priority) {}

    std::shared_ptr<std::function<void()>> toRun;
    size_t priority;

    bool operator<(const PriorityTask &b) const {
        return priority < b.priority;
    }
};

enum WorkerState {
    IDLE, RUNNING, STOPPED
};

class PriorityWorker {
public:
    PriorityWorker(const size_t &id) : id(id) {
        mainThread = workerThread();
    }

    ~PriorityWorker() {
        if (mainThread.joinable()) {
            join();
        }
    }

    void assignTask(const std::shared_ptr<PriorityTask> &task) {
        currentTask = task;
        runTask();
    }

    const size_t getId() {
        return id;
    }

    void stop() {
        state = STOPPED;
        mainSemaphore.unlock();
    }

    void join() {
        mainThread.join();
    }

    void detach() {
        mainThread.detach();
    }

    void onIdle(const std::shared_ptr<std::function<void(PriorityWorker *worker)>> &callback) {
        onIdleCallback = callback;
    }

    void onStop(const std::shared_ptr<std::function<void(PriorityWorker *worker)>> &callback) {
        onStopCallback = callback;
    }

    WorkerState state = IDLE;
private:
    const std::size_t id;

    std::thread workerThread() {
        return std::thread([&] {
            while (state != STOPPED) {
                mainSemaphore.lock();
                if (currentTask == nullptr) {
                    continue;
                }
                if (state == STOPPED) {
                    fireStopCallback();
                    return;
                }
                currentTask->toRun->operator()();
                currentTask = nullptr;
                state = IDLE;
                fireIdleCallback();
            }
        });
    }

    void runTask() {
        state = RUNNING;
        mainSemaphore.unlock();
    }

    void fireStopCallback() {
        if (onStopCallback != nullptr) {
            onStopCallback->operator()(this);
        }
    }

    void fireIdleCallback() {
        if (onIdleCallback != nullptr) {
            onIdleCallback->operator()(this);
        }
    }

    Semaphore mainSemaphore;
    std::thread mainThread;
    std::shared_ptr<PriorityTask> currentTask;
    std::shared_ptr<std::function<void(PriorityWorker *worker)>> onIdleCallback;
    std::shared_ptr<std::function<void(PriorityWorker *worker)>> onStopCallback;
};