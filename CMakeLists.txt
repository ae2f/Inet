cmake_minimum_required(VERSION 3.20)
enable_testing()
include(cmake/Core.cmake)

project(Poll VERSION 0.0.0 LANGUAGES C CXX)
if(NOT TARGET ${PROJECT_NAME})
    ae2f_CoreLibFetch(ae2f Core Rel-v6.18.11)

    ae2f_CoreLibTent(
        ${PROJECT_NAME}
        INTERFACE
        include ae2f
        "include/ae2f/Poll.h"
    )

    target_link_libraries(${PROJECT_NAME} INTERFACE Core)
endif()

project(Inet VERSION 0.0.0 LANGUAGES C CXX)

if(NOT TARGET ${PROJECT_NAME})
    ae2f_CoreLibTent(
        ${PROJECT_NAME} 
        INTERFACE 
        include ae2f
        "include/ae2f/Inet.h"
        "include/ae2f/Inet/Sock.h"
        "include/ae2f/Inet/Wsa.h"
    )

    target_link_libraries(${PROJECT_NAME} INTERFACE Poll)
endif()