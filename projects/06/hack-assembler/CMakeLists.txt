cmake_minimum_required(VERSION 3.10)
project(hack-assembler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (POLICY CMP0135)
cmake_policy(SET CMP0135 NEW)
endif (POLICY CMP0135)
include(FetchContent)
FetchContent_Declare(
absl
URL https://github.com/abseil/abseil-cpp/archive/refs/heads/master.zip
)
set(ABSL_PROPAGATE_CXX_STD ON)
FetchContent_MakeAvailable(absl)

add_executable(assembler src/main.cpp)
target_link_libraries(assembler absl::log absl::strings)
