cmake_minimum_required(VERSION 3.30)
project(https_server)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)

if (OPENSSL_FOUND)
    message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")
    message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenSSL not found!")
endif()

set(SOURCES
        main.cpp
        src/HandlingSSL.cpp
        src/HandlingReq_ProvidingResp.cpp
)

include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(https_server ${SOURCES})

target_include_directories(https_server PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib #temporary exists until i finish this shitty proj if i won't use it
)

target_link_libraries(https_server ${OPENSSL_LIBRARIES})