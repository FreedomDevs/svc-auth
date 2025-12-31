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
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_NATIVE "-O3 -DNDEBUG -march=native")


set(BUILD_EXAMPLES OFF)
set(BUILD_CTL OFF)
set(BUILD_ORM ON)
set(BUILD_POSTGRESQL ON)
set(BUILD_BROTLI OFF)
set(BUILD_YAML_CONFIG OFF)
set(WITH_ZLIB OFF)

add_subdirectory(libs/drogon)

# Добавляем include/ как include
include_directories(${PROJECT_SOURCE_DIR}/include)

# Компилируем все .cpp файлы в src
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(svc-auth ${SOURCES})

target_compile_options(svc-auth PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(svc-auth PRIVATE drogon)


