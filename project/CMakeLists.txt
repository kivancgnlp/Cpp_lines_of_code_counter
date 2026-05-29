cmake_minimum_required(VERSION 4.0)
project(disk_usage_analyzer)

set(CMAKE_CXX_STANDARD 20)

add_executable(disk_usage_analyzer
        src/main.cpp
        src/Stats.cpp
        src/Stats.h
        src/Utils.cpp
        src/Utils.h)


find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(disk_usage_analyzer PRIVATE CLI11::CLI11)

find_package(cpptrace CONFIG REQUIRED)
target_link_libraries(disk_usage_analyzer PRIVATE cpptrace::cpptrace)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(disk_usage_analyzer PRIVATE fmt::fmt)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(disk_usage_analyzer PRIVATE spdlog::spdlog)