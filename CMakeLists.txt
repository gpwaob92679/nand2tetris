cmake_minimum_required(VERSION 3.20)
project(nand2tetris LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif (POLICY CMP0135)
include(FetchContent)

FetchContent_Declare(
  absl
  URL https://github.com/abseil/abseil-cpp/archive/42624b3d9d56ae6fd5c48b28715044667942b9d8.zip
  URL_HASH SHA256=87e5f2a702c7b1bb496438d155cc44419c05bab9523ec7dba73842e566f41446
)
set(ABSL_PROPAGATE_CXX_STD ON)
FetchContent_MakeAvailable(absl)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/6a5938233b6519ba99ddb7c7314d45d3fa877969.zip
  URL_HASH SHA256=d945aea6b277d607fbcd719aef4e1e159ddd24e1be7cd34c03bd8888b47ca079
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

enable_testing()

if(WIN32)
  set(CPUEmulator "${CMAKE_SOURCE_DIR}/tools/CPUEmulator.bat")
  set(TextComparer "${CMAKE_SOURCE_DIR}/tools/TextComparer.bat")
else()
  set(CPUEmulator "${CMAKE_SOURCE_DIR}/tools/CPUEmulator.sh")
  set(TextComparer "${CMAKE_SOURCE_DIR}/tools/TextComparer.sh")
endif()

add_subdirectory(projects/06/assembler)
add_subdirectory(projects/08/vmtranslator)
add_subdirectory(projects/10/jackanalyzer)
