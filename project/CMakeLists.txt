cmake_minimum_required(VERSION 4.0)
project(cpp_loc_counter)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp_loc_counter
        src/main.cpp)


find_package(CLI11 CONFIG REQUIRED)
target_link_libraries(cpp_loc_counter PRIVATE CLI11::CLI11)

find_package(cpptrace CONFIG REQUIRED)
target_link_libraries(cpp_loc_counter PRIVATE cpptrace::cpptrace)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(cpp_loc_counter PRIVATE fmt::fmt)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(cpp_loc_counter PRIVATE spdlog::spdlog)