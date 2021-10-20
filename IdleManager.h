#pragma once

#include <queue>
#include <vector>
#include <map>
#include <memory>
#include "IdleObject.h"
#include <FlowUtils/Semaphore.h>
//#include <FlowUtils/LifetimeClock.h>


template<class IdleType>
class IdleManager {
public:
    void add(std::shared_ptr<IdleType> object) {
        objectMap[idCounter] = object;
        idleQueue.emplace(idCounter);
        idCounter++;
    }
    std::unique_ptr<IdleObject<IdleType>> get() {
//        LifetimeClock_Seconds lifetimeClock;
        std::lock_guard<std::mutex> lg(getMutex);
        emptySemaphore.wait();
        idleMutex.lock();
        const auto item = idleQueue.front();
        idleQueue.pop();
        idleMutex.unlock();
        if(idleQueue.empty())
            emptySemaphore.addLock();
        const auto mapitem = objectMap.at(item);

        return std::make_unique<IdleObject<IdleType>>(item, mapitem, [&](const size_t id){
            std::lock_guard<std::mutex> lg(idleMutex);
            toIdle(id);
        });
    }

    std::vector<std::shared_ptr<IdleType>> releaseAll() {
        std::vector<std::shared_ptr<IdleType>> rtn;
        for(auto item : objectMap){
            rtn.emplace_back(item.second);
        }
        objectMap.clear();
        return rtn;
    }

private:
    void toIdle(const size_t id){
        idleQueue.emplace(id);
        emptySemaphore.unlock();
    }
    size_t idCounter = 0;
    std::unordered_map<size_t, std::shared_ptr<IdleType>> objectMap;
//    std::queue<size_t> idleQueue;
    std::queue<size_t> idleQueue;
    std::mutex getMutex;
    std::mutex idleMutex;
    Semaphore emptySemaphore;
    int gone = 0;
};