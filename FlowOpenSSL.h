#pragma once

#include <openssl/sha.h>

namespace FlowOpenSSL {

    inline static std::vector<unsigned char> sha1(std::string input) {

        unsigned char digest[SHA_DIGEST_LENGTH];
        std::vector<unsigned char> rtn;

        SHA_CTX context;
        if (!SHA1_Init(&context))
            return rtn;

        if (!SHA1_Update(&context, (unsigned char *) input.c_str(), input.size()))
            return rtn;

        if (!SHA1_Final(digest, &context))
            return rtn;

        rtn.assign(digest, digest + SHA_DIGEST_LENGTH);

        return rtn;
    }

};
