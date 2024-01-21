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
  single_file_test_programs
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
  multi_file_test_programs
  ../FunctionCalls/NestedCall/
  ../FunctionCalls/FibonacciElement/
  ../FunctionCalls/StaticsTest/
)

function(add_vm_test path is_multi_file)
  # Note: `cmake_path()` crashes on Windows if any path contains CJK characters.
  get_filename_component(path "${path}" ABSOLUTE)
  file(RELATIVE_PATH identifier "${CMAKE_SOURCE_DIR}" "${path}")
  message(STATUS "Adding VM test program: ${identifier}")

  get_filename_component(basename "${path}" NAME_WE)
  message(DEBUG "basename: ${basename}")
  file(
    COPY "${path}/"
    DESTINATION "test_programs/${basename}/"
    PATTERN "*.asm" EXCLUDE
    PATTERN "*VME.tst" EXCLUDE
  )

  if (is_multi_file)
    set(source "test_programs/${basename}/")
  else()
    set(source "test_programs/${basename}/${basename}.vm")
  endif()
  add_test(
    NAME "vmtranslator: Translate ${identifier}"
    COMMAND vmtranslator "${source}"
  )
  add_test(
    NAME "vmtranslator: Compare ${identifier}"
    COMMAND "${CPUEmulator}" "test_programs/${basename}/${basename}.tst"
  )

  set_tests_properties(
    "vmtranslator: Translate ${identifier}"
    PROPERTIES
      FIXTURES_SETUP "vmtranslator: ${identifier}"
  )
  set_tests_properties(
    "vmtranslator: Compare ${identifier}"
    PROPERTIES
      FIXTURES_REQUIRED "vmtranslator: ${identifier}"
      PASS_REGULAR_EXPRESSION "End of script - Comparison ended successfully"
  )
endfunction()

foreach(program ${single_file_test_programs})
  add_vm_test("${program}" FALSE)
endforeach()
foreach(program ${multi_file_test_programs})
  add_vm_test("${program}" TRUE)
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
