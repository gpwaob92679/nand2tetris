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

# Tests

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
