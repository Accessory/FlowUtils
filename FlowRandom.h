#pragma once

#include <random>
#pragma warning(push)
#pragma warning(disable: 4244 4267)

namespace FlowRandom {

    inline const double randomM1t1() {
        static std::mt19937 generator(std::time(nullptr));
        static std::uniform_real_distribution<double> distribution(-1.0, 1.0);
        return distribution(generator);
    }

    inline const double random0t1() {
        static std::mt19937 generator(std::time(nullptr));
        static std::uniform_real_distribution<double> distribution(0, 1.0);
        return distribution(generator);
    }

    inline double RandomAdd(const double weight, const double min, const double max) {
        static std::mt19937 generator(std::time(nullptr));
        std::uniform_real_distribution<double> distribution(min, max);
        return weight + distribution(generator);
    }

    inline double Random(const double min, const double max) {
        static std::mt19937 generator(std::time(nullptr));
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(generator);
    }

    inline int RandomInt(const int min, const int max) {
        static std::mt19937 generator(std::time(nullptr));
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(generator);
    }

    inline size_t RandomSizeT(const int max) {
        static std::mt19937 generator(std::time(nullptr));
        std::uniform_int_distribution<size_t> distribution(0, max);
        return distribution(generator);
    }

    inline unsigned char randomChar() {
        return FlowRandom::RandomSizeT(255);
    }

    inline bool RandomTrue(double chance, double in) {
        static std::mt19937 generator(std::time(nullptr));
        std::uniform_real_distribution<double> distribution(0, in);
        return chance > distribution(generator);
    }

    inline bool RandomTrue(double chance) {
        static std::mt19937 generator(std::time(nullptr));
        std::uniform_real_distribution<double> distribution(0, 1);
        return chance > distribution(generator);
    }

    inline double NormalDistribution(double mean = 0, double variation = 1) {
        static std::mt19937 generator(std::time(nullptr));
        std::normal_distribution<double> distribution(mean, variation);
        return distribution(generator);
    }

    inline static const std::string
            alphaNum =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    inline static std::vector<unsigned char> getRandomBytes(const size_t lenght = 32){
        std::vector<unsigned char> bytes;
        bytes.reserve(lenght);
        for(size_t i = 0; i < lenght; ++i){
            bytes.emplace_back(randomChar());
        }
        return bytes;
    }

    inline static std::string getRandomString(size_t len = 5, const std::string &charSet = "") {
        std::string rtn;

        rtn.reserve(5);
        if (charSet.empty()) {
            for (; len != 0; --len) {
                rtn += alphaNum.at(RandomSizeT(alphaNum.length() - 1));
            }
        } else {
            for (; len != 0; --len) {
                rtn += charSet.at(RandomSizeT(charSet.length() - 1));
            }
        }
        return rtn;
    }
}
#pragma warning( pop )