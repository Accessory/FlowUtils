#pragma once

#include <string>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace FlowUUID {

    boost::uuids::string_generator gen;

    inline boost::uuids::uuid UUID() {
        return boost::uuids::random_generator()();
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


