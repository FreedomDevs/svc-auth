cmake_minimum_required(VERSION 4.1.0)
project(svc-auth VERSION 1.0 LANGUAGES CXX)

# Экспортируем инструкцию по сборке проекта для всяких IDE
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Используем C++ 2023
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s")


set(BUILD_EXAMPLES OFF)
set(BUILD_CTL OFF)

set(BUILD_ORM ON)
set(BUILD_POSTGRESQL ON)
set(BUILD_MYSQL OFF)
set(BUILD_SQLITE OFF)

set(BUILD_BROTLI OFF)
set(BUILD_YAML_CONFIG OFF)
set(WITH_ZLIB OFF)

add_subdirectory(libs/drogon)

set(JWT_BUILD_EXAMPLES OFF)
add_subdirectory(libs/jwt-cpp)

find_package(OpenSSL REQUIRED)

# Компилируем все .cpp файлы в src
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(svc-auth ${SOURCES})

target_include_directories(svc-auth PUBLIC ${PROJECT_SOURCE_DIR}/include ${OPENSSL_INCLUDE_DIR})
target_compile_options(svc-auth PRIVATE -Wall -Wextra -Wpedantic -fno-modules)
target_link_libraries(svc-auth PRIVATE drogon argon2 jwt-cpp ${OPENSSL_LIBRARIES})
