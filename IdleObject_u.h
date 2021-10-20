#pragma once

#include <functional>
#include <memory>

template<class IdleType>
class IdleObject_u {
public:
    IdleObject_u(const size_t &id,
               IdleType& object,
               const std::function<void(size_t)> &cb) : id(id),
//                                                        object(object),
                                                        finishedCb(cb) {
        object = object;
    }

    ~IdleObject_u() {
        finishedCb(id);
    }

    IdleType* object;
private:
    const size_t id;
    std::function<void(size_t)> finishedCb;
};