cmake_minimum_required(VERSION 3.10)
project(hack-assembler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(absl REQUIRED)

add_executable(assembler src/main.cpp)
target_link_libraries(assembler absl::log absl::strings)
