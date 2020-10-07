#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>

class MultiSemaphore {
public:

    void lock() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++lockCount;
        while (lockCount > 1) {
            condition_.wait(lock);
        }
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (lockCount > 1) {
            condition_.wait(lock);
        }
    }

    void unlock() {
        std::lock_guard<std::mutex> lg(mutex_);
        if (lockCount != 0)
            --lockCount;
        if (lockCount == 0)
            condition_.notify_all();
    }

    void addLock() {
        ++lockCount;
    }

    size_t count() {
        return lockCount;
    }

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic_size_t lockCount = 0;
};