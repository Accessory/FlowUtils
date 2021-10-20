#pragma once

#include <string>
#include <regex>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace FlowUUID {

    inline boost::uuids::string_generator gen;
    inline const static std::string UUID_REGEX_STRING = "[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89ABab][0-9a-fA-F]{3}-[0-9a-fA-F]{12}";
    inline boost::uuids::uuid UUID() {
        return boost::uuids::random_generator()();
    }

    inline bool IsUUID(const std::string &uuid) {
        const std::regex UUID_REGEX(UUID_REGEX_STRING);
        return std::regex_match(uuid, UUID_REGEX);
    }

    inline std::string UUID_String() {
        return boost::uuids::to_string(boost::uuids::random_generator()());
    }

    inline boost::uuids::uuid ToUUID(const std::string &uuid) {
        if (uuid.empty())
            return boost::uuids::nil_uuid();
        try {
            return gen(uuid);
        } catch (...) {
            return boost::uuids::nil_uuid();
        }
    }

    inline std::string ToString(const boost::uuids::uuid &uuid) {
        return boost::uuids::to_string(uuid);
    }

};


