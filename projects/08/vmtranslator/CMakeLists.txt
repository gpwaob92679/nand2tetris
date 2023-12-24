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
  absl::flags
  absl::flags_parse
  absl::flags_usage
  absl::log
  absl::strings
  absl::str_format
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

if(WIN32)
  set(CPUEmulator ${CMAKE_SOURCE_DIR}/tools/CPUEmulator.bat)
else()
  set(CPUEmulator ${CMAKE_SOURCE_DIR}/tools/CPUEmulator.sh)
endif()

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
    NAME
      "Translation: ${program}"
    COMMAND
      vmtranslator test_programs/${basename}/${basename}.vm
  )
  add_test(
    NAME
      "Comparison: ${program}"
    COMMAND
      ${CPUEmulator} test_programs/${basename}/${basename}.tst
  )
  set_tests_properties(
    "Comparison: ${program}"
    PROPERTIES
      DEPENDS "Translation: ${program}"
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
    NAME
      "Translation: ${program}"
    COMMAND
      vmtranslator test_programs/${basename}/
  )
  add_test(
    NAME
      "Comparison: ${program}"
    COMMAND
      ${CPUEmulator} test_programs/${basename}/${basename}.tst
  )
  set_tests_properties(
    "Comparison: ${program}"
    PROPERTIES
      DEPENDS "Translation: ${program}"
      PASS_REGULAR_EXPRESSION "End of script - Comparison ended successfully"
  )
endforeach()
