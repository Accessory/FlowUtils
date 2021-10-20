#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>

class Semaphore {
public:

    void addLock() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++lockCount;
        if (lockCount > 2) {
            lockCount = 2;
        }
    }
    void lock() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++lockCount;
        while (lockCount > 1) {
            condition_.wait(lock);
        }
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (lockCount > 0) {
            condition_.wait(lock);
        }
    }

    void unlock() {
        std::lock_guard<std::mutex> lg(mutex_);
        if (lockCount != 0)
            --lockCount;
        condition_.notify_all();
    }

    void unlock_one() {
        std::lock_guard<std::mutex> lg(mutex_);
        if (lockCount != 0)
            --lockCount;
        condition_.notify_one();
    }

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic_size_t lockCount = 0;
};