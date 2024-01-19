message(STATUS "Configuring project 8: VM translator")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

add_library(
  vmtranslator_addressing
  addressing.cpp
)
target_link_libraries(
  vmtranslator_addressing
  absl::log
  absl::str_format
)

add_library(
  vmtranslator_commands
  commands.cpp
)
target_link_libraries(
  vmtranslator_commands
  absl::strings
  absl::str_format
  vmtranslator_addressing
)

add_library(
  vmtranslator_parser
  parser.cpp
)
target_link_libraries(
  vmtranslator_parser
  absl::check
  absl::log
  absl::strings
  absl::str_format
  vmtranslator_addressing
  vmtranslator_commands
)

add_library(
  vmtranslator_output
  output.cpp
)
target_link_libraries(
  vmtranslator_output
  absl::check
  absl::log
  vmtranslator_commands
)

add_executable(
  vmtranslator
  main.cpp
)

target_link_libraries(
  vmtranslator
  absl::check
  absl::flags
  absl::flags_parse
  absl::flags_usage
  absl::log
  absl::strings
  absl::str_format
  vmtranslator_commands
  vmtranslator_output
  vmtranslator_parser
)

install(
  TARGETS vmtranslator
  DESTINATION ${CMAKE_SOURCE_DIR}/projects/bin
)

# Unit tests

add_executable(
  addressing_test
  addressing_test.cpp
)
target_link_libraries(
  addressing_test
  vmtranslator_addressing
  absl::str_format
  GTest::gtest_main
)
gtest_discover_tests(
  addressing_test
  TEST_PREFIX "vmtranslator::"
)

add_executable(
  commands_test
  commands_test.cpp
)
target_link_libraries(
  commands_test
  vmtranslator_commands
  GTest::gtest_main
)
gtest_discover_tests(
  commands_test
  TEST_PREFIX "vmtranslator::"
)

# Test programs

set(
  test_programs
  ../../07/StackArithmetic/SimpleAdd/
  ../../07/StackArithmetic/StackTest/
  ../../07/MemoryAccess/BasicTest/
  ../../07/MemoryAccess/PointerTest/
  ../../07/MemoryAccess/StaticTest/
  ../ProgramFlow/BasicLoop/
  ../ProgramFlow/FibonacciSeries/
  ../FunctionCalls/SimpleFunction/
)

set(
  full_test_programs
  ../FunctionCalls/NestedCall/
  ../FunctionCalls/FibonacciElement/
  ../FunctionCalls/StaticsTest/
)

foreach(program ${test_programs})
  cmake_path(REMOVE_FILENAME program)
  message(STATUS "Adding test VM program: " ${program})

  cmake_path(GET program PARENT_PATH parent_path)
  cmake_path(GET parent_path FILENAME basename)
  message(DEBUG "basename: " ${basename})

  file(
    COPY ${program}
    DESTINATION test_programs/${basename}/
    PATTERN "*.asm" EXCLUDE
    PATTERN "*VME.tst" EXCLUDE
  )

  add_test(
    NAME "vmtranslator: Translate ${program}"
    COMMAND
      vmtranslator test_programs/${basename}/${basename}.vm
  )
  add_test(
    NAME
      "vmtranslator: Compare ${program}"
    COMMAND
      ${CPUEmulator} test_programs/${basename}/${basename}.tst
  )

  set_tests_properties(
    "vmtranslator: Translate ${program}"
    PROPERTIES
      FIXTURES_SETUP "vmtranslator: ${program}"
  )
  set_tests_properties(
    "vmtranslator: Compare ${program}"
    PROPERTIES
      FIXTURES_REQUIRED "vmtranslator: ${program}"
      PASS_REGULAR_EXPRESSION "End of script - Comparison ended successfully"
  )
endforeach()

foreach(program ${full_test_programs})
  cmake_path(REMOVE_FILENAME program)
  message(STATUS "Adding test VM program: " ${program})

  cmake_path(GET program PARENT_PATH parent_path)
  cmake_path(GET parent_path FILENAME basename)
  message(DEBUG "basename: " ${basename})

  file(
    COPY ${program}
    DESTINATION test_programs/${basename}/
    PATTERN "*.asm" EXCLUDE
    PATTERN "*VME.tst" EXCLUDE
  )

  add_test(
    NAME "vmtranslator: Translate ${program}"
    COMMAND vmtranslator "test_programs/${basename}/"
  )
  add_test(
    NAME "vmtranslator: Compare ${program}"
    COMMAND "${CPUEmulator}" "test_programs/${basename}/${basename}.tst"
  )

  set_tests_properties(
    "vmtranslator: Translate ${program}"
    PROPERTIES
      FIXTURES_SETUP "vmtranslator: ${program}"
  )
  set_tests_properties(
    "vmtranslator: Compare ${program}"
    PROPERTIES
      FIXTURES_REQUIRED "vmtranslator: ${program}"
      PASS_REGULAR_EXPRESSION "End of script - Comparison ended successfully"
  )
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
