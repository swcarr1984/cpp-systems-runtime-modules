cmake_minimum_required(VERSION 3.10)
project(BoostIPCExample)

find_package(Boost REQUIRED COMPONENTS interprocess)

add_executable(process_a process_a.cpp)
target_link_libraries(process_a Boost::interprocess)

add_executable(process_b process_b.cpp)
target_link_libraries(process_b Boost::interprocess)
