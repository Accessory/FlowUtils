#include <argon2.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <vector>
#include <string>

namespace FlowArgon2 {

    const static auto FLOWARGON2_HASHLEN = 32;
    const static auto FLOWARGON2_BUFFER_LENGTH = 254;

    inline std::string encode(const std::string &input, const std::string &salt) {

        const auto salt_len = salt.size();
//        uint8_t hash1[FLOWARGON2_HASHLEN];
        char buffer[FLOWARGON2_BUFFER_LENGTH];
//        uint8_t salt[SALTLEN];
//        memset(salt, salt.data(), SALTLEN);

        uint8_t *slt = (uint8_t *) strdup(salt.data());
        uint8_t *pwd = (uint8_t *) strdup(input.data());
        uint32_t pwdlen = input.size();

        uint32_t t_cost = 3;            // 1-pass computation
        uint32_t m_cost = (1 << 16);      // 64 mebibytes memory usage
        uint32_t parallelism = 2;       // number of threads and lanes

        // high-level API
        int rslt = argon2i_hash_encoded(t_cost, m_cost, parallelism, pwd, pwdlen, slt, salt_len, FLOWARGON2_HASHLEN,
                                        buffer,
                                        FLOWARGON2_BUFFER_LENGTH);
        if (rslt) {
            std::cout << "Error encoding password";
        }
        const auto encoded_length = argon2_encodedlen(t_cost, m_cost, parallelism, salt_len, FLOWARGON2_HASHLEN,
                                                      Argon2_type::Argon2_i);
        return std::string(buffer, buffer + encoded_length - 1);
    }

    inline bool verify(const std::string &encoded_hash, const std::string &password) {

        uint8_t *pwd = (uint8_t *) strdup(password.data());
        uint32_t pwdlen = password.size();
        return argon2i_verify(encoded_hash.data(), pwd, pwdlen) == ARGON2_OK;
    }

};