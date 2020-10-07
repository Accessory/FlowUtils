#pragma once

#include <thread>
#include <functional>
#include "Semaphore.h"

enum WorkerState {
    IDLE, RUNNING, STOPPED
};

class Worker {
public:
    Worker(const size_t &id) : id(id) {
        mainThread = workerThread();
    }

    ~Worker() {
        if (mainThread.joinable()) {
            join();
        }
    }

    void assignTask(std::shared_ptr<std::function<void()>> task) {
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

    void onIdle(const std::shared_ptr<std::function<void(Worker *worker)>> &callback) {
        onIdleCallback = callback;
    }

    void onStop(const std::shared_ptr<std::function<void(Worker *worker)>> &callback) {
        onStopCallback = callback;
    }

    WorkerState state = IDLE;
private:
    const std::size_t id;

    std::thread workerThread() {
        return std::thread([&] {
            while (state != STOPPED) {
                mainSemaphore.lock();
                if(currentTask == nullptr){
                    continue;
                }
                if (state == STOPPED) {
                    fireStopCallback();
                    return;
                }
                currentTask->operator()();
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
    std::shared_ptr<std::function<void()>> currentTask;
    std::shared_ptr<std::function<void(Worker *worker)>> onIdleCallback;
    std::shared_ptr<std::function<void(Worker *worker)>> onStopCallback;
};