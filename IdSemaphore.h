#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include "Semaphore.h"

class IdSemaphore {
public:
    void addLock(const std::string &id) {
        mainLock.lock();
        if (semaphreMap.contains(id)) {

        }
        mainLock.unlock();
    }

    void lock(const std::string &id) {
        if (semaphreMap.contains(id)) {
            semaphreMap.at(id).lock();
        } else {
            mainLock.lock();
            semaphreMap[id].lock();
            mainLock.unlock();
        }
    }

    void wait(const std::string &id) {
        if (semaphreMap.contains(id)) {
            mainLock.wait();
        }
    }

    void unlock(const std::string &id) {
        mainLock.lock();
        if (semaphreMap.contains(id)) {
            semaphreMap.at(id).unlock();
            semaphreMap.erase(id);
        }
        mainLock.unlock();
    }

//    void unlock_one(const std::string &id) {
//        mainLock.lock();
//        if (semaphreMap.contains(id)) {
//
//        }
//        mainLock.unlock();
//    }

private:
    Semaphore mainLock;
    std::unordered_map<std::string, Semaphore> semaphreMap;
    std::condition_variable condition_;
    std::atomic_size_t lockCount = 0;
};