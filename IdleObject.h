#pragma once

#include <functional>
#include <memory>

template<class IdleType>
class IdleObject {
public:
    IdleObject(const size_t &id,
               std::shared_ptr<IdleType> object,
               const std::function<void(size_t)> &cb) : id(id),
                                                        object(object),
                                                        finishedCb(cb) {

    }

    ~IdleObject() {
        finishedCb(id);
    }

    std::shared_ptr<IdleType> object;
private:
    const size_t id;
    std::function<void(size_t)> finishedCb;
};