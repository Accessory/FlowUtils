#pragma once

#include <queue>
#include <vector>
#include <map>
#include <memory>
#include "IdleObject_u.h"
#include <FlowUtils/Semaphore.h>


template<class IdleType>
class IdleManager_u {
public:
    void add(IdleType object) {
        objectMap[idCounter] = object;
        idleQueue.emplace(idCounter);
        idCounter++;
    }
    std::unique_ptr<IdleObject_u<IdleType>> get() {
        std::lock_guard<std::mutex> lg(getMutex);
        emptySemaphore.wait();
        const auto item = idleQueue.front();
        idleQueue.pop();
        if(!idleQueue.empty())
            emptySemaphore.addLock();
        auto mapitem = objectMap.at(item);
        return std::make_unique<IdleObject_u<IdleType>>(item, mapitem, [&](const size_t id){
            toIdle(id);
        });
    }

    std::vector<IdleType> releaseAll() {
        std::vector<IdleType> rtn;
        for(auto item : objectMap){
            rtn.emplace_back(std::move(item.second));
        }
        objectMap.clear();
        return rtn;
    }

private:
    void toIdle(const size_t id){
        idleQueue.emplace(id);
        emptySemaphore.unlock_one();
    }
    size_t idCounter;
    std::unordered_map<size_t, IdleType> objectMap;
    std::queue<size_t> idleQueue;
    std::mutex getMutex;
    Semaphore emptySemaphore;
    int gone = 0;
};