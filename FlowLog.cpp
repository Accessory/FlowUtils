#include <FlowUtils/FlowLog.h>
#include <boost/dll.hpp>

BOOST_DLL_ALIAS(logging::setLogLevel,  // <-- this function is exported with...
                setLogLevel            // <-- ...this alias name
)

BOOST_DLL_ALIAS(logging::setLogFile,  // <-- this function is exported with...
                setLogFile            // <-- ...this alias name
)