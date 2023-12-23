add_library(
  addressing
  addressing.cpp
)
target_link_libraries(
  addressing
  absl::log
  absl::str_format
)

add_library(
  commands
  commands.cpp
)
target_link_libraries(
  commands
  absl::strings
  absl::str_format
  addressing
)

add_library(
  parser
  parser.cpp
)
target_link_libraries(
  parser
  absl::check
  absl::log
  absl::strings
  absl::str_format
  addressing
  commands
)

add_executable(
  vmtranslator
  main.cpp
)

target_link_libraries(
  vmtranslator
  absl::check
  absl::log
  absl::strings
  commands
  parser
)

# Unit tests

add_executable(
  addressing_test
  addressing_test.cpp
)
target_link_libraries(
  addressing_test
  addressing
  GTest::gtest_main
)
gtest_discover_tests(addressing_test)

add_executable(
  commands_test
  commands_test.cpp
)
target_link_libraries(
  commands_test
  commands
  GTest::gtest_main
)
gtest_discover_tests(commands_test)

# Test programs

set(
  program_tests
  ../StackArithmetic/SimpleAdd/
  ../StackArithmetic/StackTest/
  ../MemoryAccess/BasicTest/
  ../MemoryAccess/PointerTest/
  ../MemoryAccess/StaticTest/
)

if(WIN32)
  set(CPUEmulator ${CMAKE_SOURCE_DIR}/tools/CPUEmulator.bat)
else()
  set(CPUEmulator ${CMAKE_SOURCE_DIR}/tools/CPUEmulator.sh)
endif()

foreach(program_test ${program_tests})
  cmake_path(REMOVE_FILENAME program_test)
  message(STATUS "Add VM program to tests: " ${program_test})

  cmake_path(
    RELATIVE_PATH program_test
    BASE_DIRECTORY ..
    OUTPUT_VARIABLE program_test_dir
  )
  message(DEBUG "program_test_dir: " ${program_test_dir})

  file(
    COPY ${program_test}
    DESTINATION program_tests/${program_test_dir}
    PATTERN "*.asm" EXCLUDE
    PATTERN "*VME.tst" EXCLUDE
  )

  cmake_path(GET program_test PARENT_PATH parent_path)
  cmake_path(GET parent_path FILENAME program_basename)
  message(DEBUG "program_basename: " ${program_basename})

  add_test(
    NAME
      "Translation: ${program_test_dir}"
    COMMAND
      vmtranslator program_tests/${program_test_dir}/${program_basename}.vm
  )
  add_test(
    NAME
      "Comparison: ${program_test_dir}"
    COMMAND
      ${CPUEmulator} program_tests/${program_test_dir}/${program_basename}.tst
  )
  set_tests_properties(
    "Comparison: ${program_test_dir}"
    PROPERTIES
      DEPENDS "Translation: ${program_test_dir}"
      PASS_REGULAR_EXPRESSION "End of script - Comparison ended successfully"
  )
endforeach()
