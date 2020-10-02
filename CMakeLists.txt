cmake_minimum_required(VERSION 3.13)
project(FlowUtils)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE
        FlowArgParser.h
        FlowCParser.h
        FlowEscape.h
        FlowFile.h
        FlowInterface.h
        FlowLog.h
        FlowParser.h
        FlowSParser.h
        FlowString.h
        FlowUUID.h
        FlowVParser.h
        MultiSemaphore.h
        Semaphore.h
        ThreadPool.h
        UrlEscape.h
        base64.h
        FlowJson.h
        FlowOpenSSL.h
        PriorityThreadPool.h
        FlowRandom.h)

add_library(FlowUtils OBJECT ${SOURCE})


set_target_properties(FlowUtils PROPERTIES PUBLIC_HEADER
       "${SOURCE}"
        )


if (UNIX)
    install(TARGETS FlowUtils
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            PUBLIC_HEADER DESTINATION include/${CMAKE_PROJECT_NAME}
            ARCHIVE DESTINATION lib
            )
endif ()