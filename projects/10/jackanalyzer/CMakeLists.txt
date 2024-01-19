message(STATUS "Configuring project 10: Jack analyzer")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

add_library(
  jackanalyzer_tokenizer
  tokenizer.cpp
)
target_link_libraries(
  jackanalyzer_tokenizer
  absl::check
  absl::log
  absl::str_format
  absl::strings
)

add_library(
  jackanalyzer_output
  output.cpp
)
target_link_libraries(
  jackanalyzer_output
  absl::check
  absl::log
  jackanalyzer_tokenizer
)

add_executable(
  jackanalyzer
  main.cpp
)
target_link_libraries(
  jackanalyzer
  absl::check
  absl::flags
  absl::flags_parse
  absl::flags_usage
  absl::log
  absl::str_format
  jackanalyzer_tokenizer
  jackanalyzer_output
)

install(
  TARGETS jackanalyzer
  DESTINATION ${CMAKE_SOURCE_DIR}/projects/bin
)

# Test programs

set(
  test_programs
  ../ArrayTest/
  ../ExpressionLessSquare/
  ../Square/
)

foreach(program ${test_programs})
  cmake_path(REMOVE_FILENAME program)
  message(STATUS "Adding test Jack program: " ${program})

  cmake_path(GET program PARENT_PATH parent_path)
  cmake_path(GET parent_path FILENAME basename)
  message(DEBUG "basename: " ${basename})

  file(
    COPY ${program}
    DESTINATION "test_programs/${basename}/"
    PATTERN "*.vm" EXCLUDE
  )

  add_test(
    NAME "jackanalyzer: Tokenize ${program}"
    COMMAND jackanalyzer "test_programs/${basename}/"
  )
  # TODO: Comparison tests

  set_tests_properties(
    "jackanalyzer: Tokenize ${program}"
    PROPERTIES
      FIXTURES_SETUP "jackanalyzer: ${program}"
  )
  # TODO: Comparison tests
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
