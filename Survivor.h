#pragma once

#include <iostream>

class Survivor {
    static inline size_t id_count = 0;
public:
    Survivor() : id(Survivor::id_count++) {
        std::cout << "Survivor " << id_count << " awakes" << std::endl;
    }

    ~Survivor() {
        std::cout << "Survivor " << id_count << " dies" << std::endl;
    }

    size_t id;
};